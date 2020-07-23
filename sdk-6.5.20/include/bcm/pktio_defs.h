/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_PKTIO_DEFS_H__
#define __BCM_PKTIO_DEFS_H__

#if defined(INCLUDE_PKTIO) || defined(BCM_LTSW_SUPPORT)

#include <bcmpkt/bcmpkt_lbhdr_defs.h>
#include <bcmpkt/bcmpkt_rxpmd_defs.h>
#include <bcmpkt/bcmpkt_trace_defs.h>
#include <bcmpkt/bcmpkt_txpmd_defs.h>

#define BCM_PKTIO_RXPMD_FID_INVALID BCMPKT_RXPMD_FID_INVALID /* Invalid BCM_PKTIO_RXPMD FID
                                                  indicator */

#define BCM_PKTIO_RXPMD_UNICAST_QUEUE   BCMPKT_RXPMD_UNICAST_QUEUE /* Cell was stored in Unicast
                                                      Queue in MMU. */

#define BCM_PKTIO_RXPMD_QUEUE_NUM   BCMPKT_RXPMD_QUEUE_NUM /* Queue number used in MMU, PBE
                                                  field */

#define BCM_PKTIO_RXPMD_CPU_COS BCMPKT_RXPMD_CPU_COS /* OVERLAY ON QUEUE_NUM Queue number
                                              used in MMU for cpu port, PBE
                                              field */

#define BCM_PKTIO_RXPMD_HGI     BCMPKT_RXPMD_HGI /* Valid only for packets which came
                                              in on Higig+ source port. Higig+
                                              moduleheader field, PBE field. */

#define BCM_PKTIO_RXPMD_MATCHED_RULE    BCMPKT_RXPMD_MATCHED_RULE /* Matched Rule, PBE field. */

#define BCM_PKTIO_RXPMD_PKT_LENGTH  BCMPKT_RXPMD_PKT_LENGTH /* DO NOT CHANGE, USED BY CMIC
                                                  HW: Packet length after all
                                                  modification. */

#define BCM_PKTIO_RXPMD_SRC_PORT_NUM    BCMPKT_RXPMD_SRC_PORT_NUM /* Source Port Number, PBE
                                                      field. */

#define BCM_PKTIO_RXPMD_INNER_VID   BCMPKT_RXPMD_INNER_VID /* Inner VID. */

#define BCM_PKTIO_RXPMD_INNER_CFI   BCMPKT_RXPMD_INNER_CFI /* Inner CFI */

#define BCM_PKTIO_RXPMD_INNER_PRI   BCMPKT_RXPMD_INNER_PRI /* Inner priority. */

#define BCM_PKTIO_RXPMD_OUTER_VID   BCMPKT_RXPMD_OUTER_VID /* Outer VID. */

#define BCM_PKTIO_RXPMD_OUTER_CFI   BCMPKT_RXPMD_OUTER_CFI /* Outer Canoncial Format
                                                  Indicator. */

#define BCM_PKTIO_RXPMD_OUTER_PRI   BCMPKT_RXPMD_OUTER_PRI /* Outer Priority. */

#define BCM_PKTIO_RXPMD_SPECIAL_PACKET_INDICATOR BCMPKT_RXPMD_SPECIAL_PACKET_INDICATOR /* Special packet. */

#define BCM_PKTIO_RXPMD_SPECIAL_PACKET_TYPE BCMPKT_RXPMD_SPECIAL_PACKET_TYPE /* Special packet type. */

#define BCM_PKTIO_RXPMD_CHANGE_DSCP BCMPKT_RXPMD_CHANGE_DSCP /* Set to 1 if the DSCP value has
                                                  been changed by the EP */

#define BCM_PKTIO_RXPMD_DSCP    BCMPKT_RXPMD_DSCP /* New DSCP value computed by the
                                              chip (EP) for the packet */

#define BCM_PKTIO_RXPMD_CHANGE_ECN  BCMPKT_RXPMD_CHANGE_ECN /* Change the ECN as indicated by
                                                  the IFP. */

#define BCM_PKTIO_RXPMD_ECN     BCMPKT_RXPMD_ECN /* New ECN value provided by the IFP. */

#define BCM_PKTIO_RXPMD_TIMESTAMP_TYPE  BCMPKT_RXPMD_TIMESTAMP_TYPE /* NEW This pretty much maps
                                                      from
                                                      EP_TO_CPU_HEADER_OVERLAY_TYPE
                                                      from the MPB0: Invalid1:
                                                      TIMESTAMP field carries
                                                      the LM packet-count for an
                                                      OAM loss measurement  
                                                      packet2:
                                                      TIMESTAMP_UPPER,TIMESTAMP
                                                      fields carry the 64-bit DM
                                                      Time-stamp for an  
                                                      OAMdelay measurement
                                                      packet3: 802.1AS timestamp
                                                      in the TIMESTAMP field */

#define BCM_PKTIO_RXPMD_TIMESTAMP   BCMPKT_RXPMD_TIMESTAMP /* Low 32 bits of timestamp. */

#define BCM_PKTIO_RXPMD_TIMESTAMP_HI    BCMPKT_RXPMD_TIMESTAMP_HI /* High 32/16bits of
                                                      timestamp. */

#define BCM_PKTIO_RXPMD_MTP_INDEX   BCMPKT_RXPMD_MTP_INDEX /* Mirror-to-Port Index, PBE
                                                  field */

#define BCM_PKTIO_RXPMD_BPDU    BCMPKT_RXPMD_BPDU /* BPDU Packet, PBE field */

#define BCM_PKTIO_RXPMD_REPLICATION BCMPKT_RXPMD_REPLICATION /* Indicates the packet is
                                                  generated as a result of
                                                  packet replication */

#define BCM_PKTIO_RXPMD_L3ONLY  BCMPKT_RXPMD_L3ONLY /* L3 IP Multicast Packet Flag, PBE
                                              field */

#define BCM_PKTIO_RXPMD_IP_ROUTED   BCMPKT_RXPMD_IP_ROUTED /* Indicates packet was routed -
                                                  UC OR MC */

#define BCM_PKTIO_RXPMD_SRC_HIGIG   BCMPKT_RXPMD_SRC_HIGIG /* Source port is Higig. */

#define BCM_PKTIO_RXPMD_SRC_HIGIG2  BCMPKT_RXPMD_SRC_HIGIG2 /* Source port is Higig2. */

#define BCM_PKTIO_RXPMD_UC_SW_COPY_DROPPED  BCMPKT_RXPMD_UC_SW_COPY_DROPPED /* Indicates all switch
                                                          copies were dropped in
                                                          MMU (for copies to CPU
                                                          port). */

#define BCM_PKTIO_RXPMD_SWITCH  BCMPKT_RXPMD_SWITCH /* Flag to indicate this is a
                                              switched packet as opposed to a
                                              mirrored packet,PBE field */

#define BCM_PKTIO_RXPMD_SD_TAG_PRESENT  BCMPKT_RXPMD_SD_TAG_PRESENT /* Indicates there is an SD
                                                      tag in the packet */

#define BCM_PKTIO_RXPMD_ING_ITAG_ACTION BCMPKT_RXPMD_ING_ITAG_ACTION /* Indicates ingress inner
                                                      VLAN tag action, PBE
                                                      field:0: do not modify, 1:
                                                      add I-VID, 2: replace
                                                      I-VID, 3: remove I-VID */

#define BCM_PKTIO_RXPMD_ING_OTAG_ACTION BCMPKT_RXPMD_ING_OTAG_ACTION /* Indicates ingress outer
                                                      VLAN tag action, PBE
                                                      field:0: do not modify, 1:
                                                      add O-VID, 2: replace
                                                      O-VID, 3: reserved */

#define BCM_PKTIO_RXPMD_ING_TAG_TYPE    BCMPKT_RXPMD_ING_TAG_TYPE /* Indicates the incoming tag
                                                      status
                                                      (INCOMING_TAG_STATUS):For
                                                      single tag device:  0:
                                                      untagged, 1: taggedFor
                                                      double tag device:  0:
                                                      untagged, 1: single
                                                      inner-tag, 2: single
                                                      outer-tag, 3: double
                                                      tagged */

#define BCM_PKTIO_RXPMD_ONE_STEP_1588_ING_UPDATE_DONE BCMPKT_RXPMD_ONE_STEP_1588_ING_UPDATE_DONE /* This field is set by
                                                          the ingress chip to
                                                          indicate to the egress
                                                          chip that 15881-step
                                                          ingress correction
                                                          update has been
                                                          applied to this
                                                          packet. Egress
                                                          chipuses this bit when
                                                          egress port
                                                          CF_UPDATE_MODE is set
                                                          to
                                                          ING_UPDATE_BASED_ENABLE.Encodings
                                                          are:0: Ingress
                                                          correction update has
                                                          NOT been applied to
                                                          the packet. When
                                                          configured   in
                                                          coupled mode, egress
                                                          chip must NOT apply
                                                          the egress correction
                                                          update for   this
                                                          packet.1: Ingress
                                                          correction update has
                                                          been applied to the
                                                          packet. When
                                                          configured in  
                                                          coupled mode, egress
                                                          chip must apply the
                                                          egress correction
                                                          update. */

#define BCM_PKTIO_RXPMD_EH_TAG  BCMPKT_RXPMD_EH_TAG /* The full 32-bit EH tag */

#define BCM_PKTIO_RXPMD_EH_TYPE BCMPKT_RXPMD_EH_TYPE /* Defines the header type enabling
                                              flexibility for future
                                              applications.0x0: Queue selections
                                              for Sirius/Arad.0x1: ClassID0x2:
                                              ClassID + QTag0x3-0x15: Reserved
                                              for future use. */

#define BCM_PKTIO_RXPMD_EH_TYPE_0_EH_SEG_SEL BCMPKT_RXPMD_EH_TYPE_0_EH_SEG_SEL /* See
                                                          EH_SEG_SEL_ENCODING
                                                          format */

#define BCM_PKTIO_RXPMD_EH_TYPE_0_EH_TAG_TYPE BCMPKT_RXPMD_EH_TYPE_0_EH_TAG_TYPE /* EH queue tag
                                                          assignment00: No EH
                                                          queue tag
                                                          assignment,01-11: EH
                                                          queue tag assignment
                                                          modes */

#define BCM_PKTIO_RXPMD_EH_TYPE_0_EH_QUEUE_TAG BCMPKT_RXPMD_EH_TYPE_0_EH_QUEUE_TAG /* EH queue tag */

#define BCM_PKTIO_RXPMD_EH_TYPE_1_CLASSID_TYPE BCMPKT_RXPMD_EH_TYPE_1_CLASSID_TYPE /* Type of CLASSID */

#define BCM_PKTIO_RXPMD_EH_TYPE_1_L3_IIF    BCMPKT_RXPMD_EH_TYPE_1_L3_IIF /* L3_IIF */

#define BCM_PKTIO_RXPMD_EH_TYPE_1_CLASSID   BCMPKT_RXPMD_EH_TYPE_1_CLASSID /* CLASSID */

#define BCM_PKTIO_RXPMD_EH_TYPE_2_CLASSID_TYPE BCMPKT_RXPMD_EH_TYPE_2_CLASSID_TYPE /* Type of CLASSID */

#define BCM_PKTIO_RXPMD_EH_TYPE_2_CLASSID   BCMPKT_RXPMD_EH_TYPE_2_CLASSID /* CLASSID */

#define BCM_PKTIO_RXPMD_EH_TYPE_2_EH_QUEUE_TAG BCMPKT_RXPMD_EH_TYPE_2_EH_QUEUE_TAG /* L3_IIF */

#define BCM_PKTIO_RXPMD_RX_BFD_START_OFFSET BCMPKT_RXPMD_RX_BFD_START_OFFSET /* Indicates the Session
                                                          ID for the Rx BFD
                                                          packet.Valid when
                                                          OAM_PKT_TYPE=2 */

#define BCM_PKTIO_RXPMD_RX_BFD_START_OFFSET_TYPE BCMPKT_RXPMD_RX_BFD_START_OFFSET_TYPE /* Indicates the Session
                                                          ID for the Rx BFD
                                                          packet.Valid when
                                                          OAM_PKT_TYPE=2 */

#define BCM_PKTIO_RXPMD_RX_BFD_SESSION_INDEX BCMPKT_RXPMD_RX_BFD_SESSION_INDEX /* Indicates the Session
                                                          ID for the Rx BFD
                                                          packet.Valid when
                                                          OAM_PKT_TYPE=2 */

#define BCM_PKTIO_RXPMD_REASON_TYPE BCMPKT_RXPMD_REASON_TYPE /* The reason code TYPE. */

#define BCM_PKTIO_RXPMD_DO_NOT_CHANGE_TTL   BCMPKT_RXPMD_DO_NOT_CHANGE_TTL /* This should be the
                                                          final control in the
                                                          EP indicating to not
                                                          change the TTL,from
                                                          any source. */

#define BCM_PKTIO_RXPMD_I2E_CLASSID_TYPE    BCMPKT_RXPMD_I2E_CLASSID_TYPE /* Valid for non-BFD
                                                          packet */

#define BCM_PKTIO_RXPMD_I2E_CLASSID BCMPKT_RXPMD_I2E_CLASSID /* Valid for non-BFD packet */

#define BCM_PKTIO_RXPMD_ING_L3_INTF BCMPKT_RXPMD_ING_L3_INTF /* Ingress L3 Intf number. */

#define BCM_PKTIO_RXPMD_LOOPBACK_PACKET_TYPE BCMPKT_RXPMD_LOOPBACK_PACKET_TYPE /* Indication to next
                                                          pass about the type of
                                                          loopback. */

#define BCM_PKTIO_RXPMD_REGEN_CRC   BCMPKT_RXPMD_REGEN_CRC /* If set, then packet has been
                                                  modified by the EP and CRC
                                                  needs to be regenerated */

#define BCM_PKTIO_RXPMD_ENTROPY_LABEL   BCMPKT_RXPMD_ENTROPY_LABEL /* Entropy label. */

#define BCM_PKTIO_RXPMD_TUNNEL_DECAP_TYPE   BCMPKT_RXPMD_TUNNEL_DECAP_TYPE /* Used to indicate if
                                                          and how a tunnel has
                                                          been decapsulated from
                                                          the packet. */

#define BCM_PKTIO_RXPMD_DLB_ID_VALID    BCMPKT_RXPMD_DLB_ID_VALID /* DLB_ID valid. */

#define BCM_PKTIO_RXPMD_DLB_ID  BCMPKT_RXPMD_DLB_ID /* DLB_ID. */

#define BCM_PKTIO_RXPMD_HG2_EXT_HDR_VALID   BCMPKT_RXPMD_HG2_EXT_HDR_VALID /* HG2 Extended Header
                                                          Valid. (was in
                                                          FRC_reserved) */

#define BCM_PKTIO_RXPMD_VNTAG_ACTION    BCMPKT_RXPMD_VNTAG_ACTION /* New field for VNTAG/ETAGs:
                                                      00: Do not Modify 01:
                                                      Add/Replace incoming
                                                      VNTAG/ETAG with Ingress
                                                      Port Based VNTAG 10:
                                                      Add/Replace incoming
                                                      VNTAG/ETAG with Ingress
                                                      Port Based ETAG. 11:
                                                      Delete Packets VNTAG/ETAG */

#define BCM_PKTIO_RXPMD_DVP_NHI_SEL BCMPKT_RXPMD_DVP_NHI_SEL /* If set, the DVP/NHOP_INDEX
                                                  overlay has the DVP. Else, it
                                                  has the Next Hop Index. */

#define BCM_PKTIO_RXPMD_VFI_VALID   BCMPKT_RXPMD_VFI_VALID /* Validates VFI field */

#define BCM_PKTIO_RXPMD_REPLICATION_OR_NHOP_INDEX BCMPKT_RXPMD_REPLICATION_OR_NHOP_INDEX /* This field's msb is
                                                          the replication type
                                                          from the EGR_IPMC
                                                          table.It is a flag
                                                          that determines the
                                                          contents of the lower
                                                          15-bits. 0 => Lower
                                                          16-bits contain the
                                                          IPMC_INTF_NUM from the
                                                          MMU 1 => Lower 16-bits
                                                          contain the
                                                          NEXT_HOP_INDEX from
                                                          the Ingress Pipeline */

#define BCM_PKTIO_RXPMD_HIGIG_LOOKUP_PKT    BCMPKT_RXPMD_HIGIG_LOOKUP_PKT /* Higig Lookup packet
                                                          Indication. */

#define BCM_PKTIO_RXPMD_VRF     BCMPKT_RXPMD_VRF /* VRF. */

#define BCM_PKTIO_RXPMD_NEXT_HOP_INDEX_B    BCMPKT_RXPMD_NEXT_HOP_INDEX_B /* NEXT_HOP_INDEX from
                                                          Underlay */

#define BCM_PKTIO_RXPMD_NEXT_HOP_INDEX_A    BCMPKT_RXPMD_NEXT_HOP_INDEX_A /* NEXT_HOP_INDEX from
                                                          Overlay */

#define BCM_PKTIO_RXPMD_NEXT_HOP_INDEX_TYPE BCMPKT_RXPMD_NEXT_HOP_INDEX_TYPE /* Indicates type info
                                                          for NEXT_HOP_INDEX_A
                                                          and _B fields */

#define BCM_PKTIO_RXPMD_FORWARDING_TYPE BCMPKT_RXPMD_FORWARDING_TYPE /* Packet forwarding type, it
                                                      is
                                                      MPB_BASE_FORWARD_FORMAT::FORWARDING_TYPE */

#define BCM_PKTIO_RXPMD_BFD_SESSION_INDEX_VALID BCMPKT_RXPMD_BFD_SESSION_INDEX_VALID /* Indicate a BFD
                                                          terminated packet. */

#define BCM_PKTIO_RXPMD_MATCH_ID_LO BCMPKT_RXPMD_MATCH_ID_LO /* Low 32 bits of the packet
                                                  MATCH_ID. */

#define BCM_PKTIO_RXPMD_MATCH_ID_HI BCMPKT_RXPMD_MATCH_ID_HI /* High 18 bits of the packet
                                                  MATCH_ID. */

#define BCM_PKTIO_RXPMD_L3_IIF  BCMPKT_RXPMD_L3_IIF /* L3_IIF. */

#define BCM_PKTIO_RXPMD_MPB_FLEX_DATA_TYPE  BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE /* MPB flex data_type
                                                          from
                                                          MPB_FIXED.flex_data_type */

#define BCM_PKTIO_RXPMD_INT_CN  BCMPKT_RXPMD_INT_CN /* From EPRE/EDEV_CONFIG table
                                              EGR_INT_CN_UPDPATE.int_cn */

#define BCM_PKTIO_RXPMD_CNG     BCMPKT_RXPMD_CNG /* From MPB_FIXED */

#define BCM_PKTIO_RXPMD_OPAQUE_CTRL_0   BCMPKT_RXPMD_OPAQUE_CTRL_0 /* From MPB_FIXED */

#define BCM_PKTIO_RXPMD_OPAQUE_CTRL_1   BCMPKT_RXPMD_OPAQUE_CTRL_1 /* From MPB_FIXED */

#define BCM_PKTIO_RXPMD_OPAQUE_CTRL_2_OR_MIRROR_ENCAP_INDEX BCMPKT_RXPMD_OPAQUE_CTRL_2_OR_MIRROR_ENCAP_INDEX /* From EPRE */

#define BCM_PKTIO_RXPMD_EGR_ZONE_REMAP_CTRL BCMPKT_RXPMD_EGR_ZONE_REMAP_CTRL /* From MPB_FIXED */

#define BCM_PKTIO_RXPMD_DMA_HEADER_VERSION  BCMPKT_RXPMD_DMA_HEADER_VERSION /* Constant specified in
                                                          NPL */

#define BCM_PKTIO_RXPMD_IM_MTP_INDEX    BCMPKT_RXPMD_IM_MTP_INDEX /* Ingress Mirror
                                                      Mirror-to-Port Index, PBE
                                                      field */

#define BCM_PKTIO_RXPMD_EM_MTP_INDEX    BCMPKT_RXPMD_EM_MTP_INDEX /* Egress Mirror
                                                      Mirror-to-Port Index, PBE
                                                      field */

#define BCM_PKTIO_RXPMD_IMIRROR BCMPKT_RXPMD_IMIRROR /* Ingress Mirroring, PBE field */

#define BCM_PKTIO_RXPMD_EMIRROR BCMPKT_RXPMD_EMIRROR /* Egress Mirroring, PBE field */

#define BCM_PKTIO_RXPMD_MULTICAST   BCMPKT_RXPMD_MULTICAST /* Multicast packet from
                                                  MPB_FIXED */

#define BCM_PKTIO_RXPMD_COPY_TO_CPU BCMPKT_RXPMD_COPY_TO_CPU /* Copy to CPU from MPB_FIXED */

#define BCM_PKTIO_RXPMD_TRUNCATE_CPU_COPY   BCMPKT_RXPMD_TRUNCATE_CPU_COPY /* Trucate CPU copy from
                                                          MPB_FIXED */

#define BCM_PKTIO_RXPMD_INCOMING_INT_HDR_TYPE BCMPKT_RXPMD_INCOMING_INT_HDR_TYPE /* Incoming INT header
                                                          type. */

#define BCM_PKTIO_RXPMD_FID_COUNT   BCMPKT_RXPMD_FID_COUNT /* RXPMD FIELD ID NUMBER */

#define BCM_PKTIO_RXPMD_TIMESTAMP_T_INVALID BCMPKT_RXPMD_TIMESTAMP_T_INVALID 

#define BCM_PKTIO_RXPMD_TIMESTAMP_T_OAM_LM  BCMPKT_RXPMD_TIMESTAMP_T_OAM_LM 

#define BCM_PKTIO_RXPMD_TIMESTAMP_T_OAM_DM  BCMPKT_RXPMD_TIMESTAMP_T_OAM_DM 

#define BCM_PKTIO_RXPMD_TIMESTAMP_T_IEEE_802_1AS BCMPKT_RXPMD_TIMESTAMP_T_IEEE_802_1AS 

#define BCM_PKTIO_RXPMD_ING_TAG_T_NONE  BCMPKT_RXPMD_ING_TAG_T_NONE /* No tag */

#define BCM_PKTIO_RXPMD_ING_TAG_T_SINGLE_INNER BCMPKT_RXPMD_ING_TAG_T_SINGLE_INNER /* Singgle inner tag */

#define BCM_PKTIO_RXPMD_ING_TAG_T_TAGGED    BCMPKT_RXPMD_ING_TAG_T_TAGGED /* Tagged */

#define BCM_PKTIO_RXPMD_ING_TAG_T_SINGLE_OUTER BCMPKT_RXPMD_ING_TAG_T_SINGLE_OUTER /* Singgle outer tag */

#define BCM_PKTIO_RXPMD_ING_TAG_T_DOUBLE    BCMPKT_RXPMD_ING_TAG_T_DOUBLE /* Double tags */

#define BCM_PKTIO_RXPMD_EH_T_0  BCMPKT_RXPMD_EH_T_0 

#define BCM_PKTIO_RXPMD_EH_T_1  BCMPKT_RXPMD_EH_T_1 

#define BCM_PKTIO_RXPMD_EH_T_2  BCMPKT_RXPMD_EH_T_2 

#define BCM_PKTIO_RX_REASON_NONE    BCMPKT_RX_REASON_NONE /* No reason */

#define BCM_PKTIO_RX_REASON_ADAPT_MISS  BCMPKT_RX_REASON_ADAPT_MISS /* Adapt miss. */

#define BCM_PKTIO_RX_REASON_CPU_IPMC_INTERFACE_MISMATCH BCMPKT_RX_REASON_CPU_IPMC_INTERFACE_MISMATCH /* IPMC interface
                                                          mismatch */

#define BCM_PKTIO_RX_REASON_NAT BCMPKT_RX_REASON_NAT /* Get: Indicate NAT REASON happened;
                                              Set: Configure NAT REASON mask. */

#define BCM_PKTIO_RX_REASON_NAT_TCP_UDP_MISS BCMPKT_RX_REASON_NAT_TCP_UDP_MISS /* TCP/UDP packet NAT
                                                          lookup miss. */

#define BCM_PKTIO_RX_REASON_NAT_ICMP_MISS   BCMPKT_RX_REASON_NAT_ICMP_MISS /* ICMP packet NAT lookup
                                                          miss. */

#define BCM_PKTIO_RX_REASON_NAT_FRAGMENT    BCMPKT_RX_REASON_NAT_FRAGMENT /* NAT lookup on
                                                          fragmented packet. */

#define BCM_PKTIO_RX_REASON_NAT_OTHER_MISS  BCMPKT_RX_REASON_NAT_OTHER_MISS /* Non TCP/UDP/ICMP
                                                          packet NAT lookup
                                                          miss. */

#define BCM_PKTIO_RX_REASON_FCOE_ZONE_CHECK_FAIL BCMPKT_RX_REASON_FCOE_ZONE_CHECK_FAIL /* FCOE zone check fail */

#define BCM_PKTIO_RX_REASON_VXLAN_VN_ID_MISS BCMPKT_RX_REASON_VXLAN_VN_ID_MISS /* VXLAN VNID miss */

#define BCM_PKTIO_RX_REASON_VXLAN_SIP_MISS  BCMPKT_RX_REASON_VXLAN_SIP_MISS /* VXLAN SIP miss */

#define BCM_PKTIO_RX_REASON_QCN_CNM_PRP_DLF BCMPKT_RX_REASON_QCN_CNM_PRP_DLF /* QCN_CNM_PRP_DLF */

#define BCM_PKTIO_RX_REASON_QCN_CNM_PRP BCMPKT_RX_REASON_QCN_CNM_PRP /* QCN_CNM_PRP */

#define BCM_PKTIO_RX_REASON_MPLS_ALERT_LABEL BCMPKT_RX_REASON_MPLS_ALERT_LABEL /* MPLS_ALERT_LABEL */

#define BCM_PKTIO_RX_REASON_MPLS_ILLEGAL_RESERVED_LABEL BCMPKT_RX_REASON_MPLS_ILLEGAL_RESERVED_LABEL /* MPLS_ILLEGAL_RESERVED_LABEL */

#define BCM_PKTIO_RX_REASON_ICNM    BCMPKT_RX_REASON_ICNM /* ICNM */

#define BCM_PKTIO_RX_REASON_PACKET_TRACE_TO_CPU BCMPKT_RX_REASON_PACKET_TRACE_TO_CPU /* Copy to CPU for
                                                          visibility packet */

#define BCM_PKTIO_RX_REASON_BFD_ERROR   BCMPKT_RX_REASON_BFD_ERROR /* BFD Error */

#define BCM_PKTIO_RX_REASON_BFD_SLOWPATH    BCMPKT_RX_REASON_BFD_SLOWPATH /* BFD Slowpath to CPU */

#define BCM_PKTIO_RX_REASON_L2GRE_VPNID_MISS BCMPKT_RX_REASON_L2GRE_VPNID_MISS /* L2GRE_VPNID_MISS */

#define BCM_PKTIO_RX_REASON_L2GRE_SIP_MISS  BCMPKT_RX_REASON_L2GRE_SIP_MISS /* L2GRE_SIP_MISS */

#define BCM_PKTIO_RX_REASON_TRILL   BCMPKT_RX_REASON_TRILL /* Get: Indicate TRILL REASON
                                                  happened; Set: Configure TRILL
                                                  REASON mask. */

#define BCM_PKTIO_RX_REASON_TRILL_HDR_ERROR BCMPKT_RX_REASON_TRILL_HDR_ERROR /* Trill Header Error */

#define BCM_PKTIO_RX_REASON_TRILL_LOOKUP_MISS BCMPKT_RX_REASON_TRILL_LOOKUP_MISS /* Trill Lookup Miss */

#define BCM_PKTIO_RX_REASON_TRILL_RPF_CHECK_FAIL BCMPKT_RX_REASON_TRILL_RPF_CHECK_FAIL /* Trill RPF Check Fail */

#define BCM_PKTIO_RX_REASON_TRILL_SLOWPATH  BCMPKT_RX_REASON_TRILL_SLOWPATH /* Trill Slow Path */

#define BCM_PKTIO_RX_REASON_TRILL_CORE_IS_IS_PKT BCMPKT_RX_REASON_TRILL_CORE_IS_IS_PKT /* Trill Core IS-IS */

#define BCM_PKTIO_RX_REASON_TRILL_HOP_COUNT_CHECK_FAIL BCMPKT_RX_REASON_TRILL_HOP_COUNT_CHECK_FAIL /* Trill Hop Count Check
                                                          Failure */

#define BCM_PKTIO_RX_REASON_TRILL_NICKNAME_TABLE BCMPKT_RX_REASON_TRILL_NICKNAME_TABLE /* Rbridge nickname
                                                          lookup copy to cpu */

#define BCM_PKTIO_RX_REASON_TUNNEL_DECAP_ECN_ERROR BCMPKT_RX_REASON_TUNNEL_DECAP_ECN_ERROR /* Tunnel decap ECN error */

#define BCM_PKTIO_RX_REASON_OAM_SLOWPATH    BCMPKT_RX_REASON_OAM_SLOWPATH /* OAM packets copied to
                                                          the CPU for slowpath
                                                          processing */

#define BCM_PKTIO_RX_REASON_TIME_SYNC   BCMPKT_RX_REASON_TIME_SYNC /* Indicates packet was
                                                      copied to CPU due to a
                                                      network time sync packet. 
                                                      This isset either by
                                                      dedicated IEEE 802.1AS
                                                      controls or the FP. */

#define BCM_PKTIO_RX_REASON_VXLT_MISS   BCMPKT_RX_REASON_VXLT_MISS /* Indicates packet was
                                                      copied to the CPU due to
                                                      VLAN Translation miss. */

#define BCM_PKTIO_RX_REASON_NIV BCMPKT_RX_REASON_NIV /* Get: Indicate NIV REASON happened;
                                              Set: Configure NIV REASON mask. */

#define BCM_PKTIO_RX_REASON_NIV_DOT1P_DROP  BCMPKT_RX_REASON_NIV_DOT1P_DROP /* Dot1p Admittance
                                                          Discard */

#define BCM_PKTIO_RX_REASON_NIV_VIF_MISS    BCMPKT_RX_REASON_NIV_VIF_MISS /* VIF Lookup Miss */

#define BCM_PKTIO_RX_REASON_NIV_RPF_MISS    BCMPKT_RX_REASON_NIV_RPF_MISS /* RPF Lookup Miss */

#define BCM_PKTIO_RX_REASON_NIV_VNTAG_ERROR BCMPKT_RX_REASON_NIV_VNTAG_ERROR /* VNTAG Format Error */

#define BCM_PKTIO_RX_REASON_NIV_VNTAG_PRESENT BCMPKT_RX_REASON_NIV_VNTAG_PRESENT /* VNTAG Present Drop */

#define BCM_PKTIO_RX_REASON_NIV_VNTAG_NOT_PRESENT BCMPKT_RX_REASON_NIV_VNTAG_NOT_PRESENT /* VNTAG Not Present Drop */

#define BCM_PKTIO_RX_REASON_MY_STATION  BCMPKT_RX_REASON_MY_STATION /* Copy to CPU for MY_STATION
                                                      match reason */

#define BCM_PKTIO_RX_REASON_MPLS_UNKNOWN_ACH_ERROR BCMPKT_RX_REASON_MPLS_UNKNOWN_ACH_ERROR /* MPLS unknown ACH */

#define BCM_PKTIO_RX_REASON_L3_NEXT_HOP BCMPKT_RX_REASON_L3_NEXT_HOP /* ING_L3_NEXT_HOP table copy
                                                      to CPU */

#define BCM_PKTIO_RX_REASON_PBT_NONUC_PKT   BCMPKT_RX_REASON_PBT_NONUC_PKT /* Provider Backbone
                                                          Transport pkt is not
                                                          unicast */

#define BCM_PKTIO_RX_REASON_MPLS_SEQ_NUM_FAIL BCMPKT_RX_REASON_MPLS_SEQ_NUM_FAIL /* Bit32 - MPLS sequence
                                                          number check fail */

#define BCM_PKTIO_RX_REASON_MPLS_TTL_CHECK_FAIL BCMPKT_RX_REASON_MPLS_TTL_CHECK_FAIL /* Bit31 - MPLS TTL check
                                                          fail */

#define BCM_PKTIO_RX_REASON_MPLS_INVALID_PAYLOAD BCMPKT_RX_REASON_MPLS_INVALID_PAYLOAD /* Bit30 - MPLS invalid
                                                          payload */

#define BCM_PKTIO_RX_REASON_MPLS_INVALID_ACTION BCMPKT_RX_REASON_MPLS_INVALID_ACTION /* Bit29 - MPLS invalid
                                                          label action */

#define BCM_PKTIO_RX_REASON_MPLS_LABEL_MISS BCMPKT_RX_REASON_MPLS_LABEL_MISS /* Bit28 - MPLS label
                                                          lookup miss */

#define BCM_PKTIO_RX_REASON_MAC_BIND_FAIL   BCMPKT_RX_REASON_MAC_BIND_FAIL /* Copy to CPU for MAC to
                                                          IP bind check failures */

#define BCM_PKTIO_RX_REASON_CBSM_PREVENTED  BCMPKT_RX_REASON_CBSM_PREVENTED /* Bit26 - CBSM_PREVENTED
                                                          - copy to CPU */

#define BCM_PKTIO_RX_REASON_CPU_VFP BCMPKT_RX_REASON_CPU_VFP /* Bit25 - VFP Action - copy to
                                                  CPU */

#define BCM_PKTIO_RX_REASON_MCIDX_ERROR BCMPKT_RX_REASON_MCIDX_ERROR /* Bit24 - Multicast index
                                                      error */

#define BCM_PKTIO_RX_REASON_HGHDR_ERROR BCMPKT_RX_REASON_HGHDR_ERROR /* Bit23 - HiGig Header error */

#define BCM_PKTIO_RX_REASON_L3_MTU_CHECK_FAIL BCMPKT_RX_REASON_L3_MTU_CHECK_FAIL /* Bit22 - L3 MTU check
                                                          fail to CPU */

#define BCM_PKTIO_RX_REASON_PARITY_ERROR    BCMPKT_RX_REASON_PARITY_ERROR /* Bit21 - Parity error
                                                          on IP tables */

#define BCM_PKTIO_RX_REASON_L3_SLOWPATH BCMPKT_RX_REASON_L3_SLOWPATH /* Bit20 - L3 slow path CPU
                                                      processed packets */

#define BCM_PKTIO_RX_REASON_ICMP_REDIRECT   BCMPKT_RX_REASON_ICMP_REDIRECT /* Bit19 - ICMP Redirect
                                                          copy to CPU */

#define BCM_PKTIO_RX_REASON_CPU_SFLOW_FLEX  BCMPKT_RX_REASON_CPU_SFLOW_FLEX /* Bit18 - Flex Sflow */

#define BCM_PKTIO_RX_REASON_CPU_TUNNEL_ERR  BCMPKT_RX_REASON_CPU_TUNNEL_ERR /* Bit17 - Tunnel error
                                                          trap to CPU */

#define BCM_PKTIO_RX_REASON_CPU_MARTIAN_ADDR BCMPKT_RX_REASON_CPU_MARTIAN_ADDR /* Bit16 - Martian
                                                          address trap to CPU */

#define BCM_PKTIO_RX_REASON_CPU_DOS_ATTACK  BCMPKT_RX_REASON_CPU_DOS_ATTACK /* Bit15 - DOS attack
                                                          trap to CPU */

#define BCM_PKTIO_RX_REASON_CPU_PROTOCOL_PKT BCMPKT_RX_REASON_CPU_PROTOCOL_PKT /* Bit14 - Protocol
                                                          Packet */

#define BCM_PKTIO_RX_REASON_CPU_L3HDR_ERR   BCMPKT_RX_REASON_CPU_L3HDR_ERR /* Bit13 - L3 header  -
                                                          IP options, TTL=0,
                                                          !IPv4 etc. */

#define BCM_PKTIO_RX_REASON_CPU_FFP BCMPKT_RX_REASON_CPU_FFP /* Bit12 - FFP Action - copy to
                                                  CPU */

#define BCM_PKTIO_RX_REASON_CPU_IPMC_MISS   BCMPKT_RX_REASON_CPU_IPMC_MISS /* Bit11 - IPMC miss -
                                                          {SIP, DIP} miss or DIP
                                                          miss */

#define BCM_PKTIO_RX_REASON_CPU_MC_MISS BCMPKT_RX_REASON_CPU_MC_MISS /* Bit10 - MC miss */

#define BCM_PKTIO_RX_REASON_CPU_L3SRC_MOVE  BCMPKT_RX_REASON_CPU_L3SRC_MOVE /* Bit9 - Station
                                                          Movement - L3 */

#define BCM_PKTIO_RX_REASON_CPU_L3DST_MISS  BCMPKT_RX_REASON_CPU_L3DST_MISS /* Bit8 - L3 DIP Miss */

#define BCM_PKTIO_RX_REASON_CPU_L3SRC_MISS  BCMPKT_RX_REASON_CPU_L3SRC_MISS /* Bit7 - L3 SIP Miss */

#define BCM_PKTIO_RX_REASON_CPU_SFLOW_DST   BCMPKT_RX_REASON_CPU_SFLOW_DST /* Bit6 - sFlow - Dst */

#define BCM_PKTIO_RX_REASON_CPU_SFLOW_SRC   BCMPKT_RX_REASON_CPU_SFLOW_SRC /* Bit5 - sFlow - Src */

#define BCM_PKTIO_RX_REASON_CPU_L2CPU   BCMPKT_RX_REASON_CPU_L2CPU /* Bit4 - L2_TABLE - copy to
                                                      CPU */

#define BCM_PKTIO_RX_REASON_CPU_L2MOVE  BCMPKT_RX_REASON_CPU_L2MOVE /* Bit3- Station Movement -
                                                      L2 */

#define BCM_PKTIO_RX_REASON_CPU_DLF BCMPKT_RX_REASON_CPU_DLF /* Bit2 - DLF, */

#define BCM_PKTIO_RX_REASON_CPU_SLF BCMPKT_RX_REASON_CPU_SLF /* Bit1 - SLF */

#define BCM_PKTIO_RX_REASON_CPU_UVLAN   BCMPKT_RX_REASON_CPU_UVLAN /* Bit0- CPU Learn bit is set
                                                      in PTABLE and SA is
                                                      learnt; unknown VLAN; VID
                                                      = 0xfff */

#define BCM_PKTIO_RX_REASON_PROTECTION_DATA_DROP BCMPKT_RX_REASON_PROTECTION_DATA_DROP /* Bit18 - protection
                                                          data dropped packet
                                                          copied to CPU */

#define BCM_PKTIO_RX_REASON_OAM_ERROR   BCMPKT_RX_REASON_OAM_ERROR /* OAM packets copied to the
                                                      CPU for error cases */

#define BCM_PKTIO_RX_REASON_OAM_LMDM    BCMPKT_RX_REASON_OAM_LMDM /* Copy to CPU for OAM LMDM */

#define BCM_PKTIO_RX_REASON_CPU_SFLOW   BCMPKT_RX_REASON_CPU_SFLOW /* Get: Indicate SFLOW REASON
                                                      happened; Set: Configure
                                                      SFLOW REASON mask. */

#define BCM_PKTIO_RX_REASON_CPU_SFLOW_CPU_SFLOW_FLEX BCMPKT_RX_REASON_CPU_SFLOW_CPU_SFLOW_FLEX /* FP based Sflow */

#define BCM_PKTIO_RX_REASON_CPU_SFLOW_CPU_SFLOW_DST BCMPKT_RX_REASON_CPU_SFLOW_CPU_SFLOW_DST /* Egress port-based
                                                          Sflow */

#define BCM_PKTIO_RX_REASON_CPU_SFLOW_CPU_SFLOW_SRC BCMPKT_RX_REASON_CPU_SFLOW_CPU_SFLOW_SRC /* Ingress port-based
                                                          Sflow */

#define BCM_PKTIO_RX_REASON_MPLS_PROC_ERROR BCMPKT_RX_REASON_MPLS_PROC_ERROR /* Get: Indicate SFLOW
                                                          REASON happened; Set:
                                                          Configure SFLOW REASON
                                                          mask. */

#define BCM_PKTIO_RX_REASON_MPLS_PROC_ERROR_INVALID_PAYLOAD BCMPKT_RX_REASON_MPLS_PROC_ERROR_INVALID_PAYLOAD /* Invalid payload. */

#define BCM_PKTIO_RX_REASON_MPLS_PROC_ERROR_INVALID_ACTION BCMPKT_RX_REASON_MPLS_PROC_ERROR_INVALID_ACTION /* Invalid action. */

#define BCM_PKTIO_RX_REASON_MPLS_PROC_ERROR_LABEL_MISS BCMPKT_RX_REASON_MPLS_PROC_ERROR_LABEL_MISS /* Lookup label miss. */

#define BCM_PKTIO_RX_REASON_MPLS_PROC_ERROR_TTL_CHECK_FAIL BCMPKT_RX_REASON_MPLS_PROC_ERROR_TTL_CHECK_FAIL /* TTL check fail. */

#define BCM_PKTIO_RX_REASON_MPLS_UNKNOWN_CONTROL_PKT BCMPKT_RX_REASON_MPLS_UNKNOWN_CONTROL_PKT /* TBD. */

#define BCM_PKTIO_RX_REASON_ETRAP_MONITOR   BCMPKT_RX_REASON_ETRAP_MONITOR /* Copied to CPU by ETRAP
                                                          monitor. */

#define BCM_PKTIO_RX_REASON_INT_TURN_AROUND BCMPKT_RX_REASON_INT_TURN_AROUND /* Copied to CPU by
                                                          Inband Telemetry
                                                          turnaround. */

#define BCM_PKTIO_RX_REASON_DLB_MONITOR BCMPKT_RX_REASON_DLB_MONITOR /* Copied to CPU by DLB
                                                      monitor. */

#define BCM_PKTIO_RX_REASON_CPU_L3_HDR_MISMATCH BCMPKT_RX_REASON_CPU_L3_HDR_MISMATCH /* Bit13 - L3 header  -
                                                          IP options, TTL=0,
                                                          !IPv4 etc. */

#define BCM_PKTIO_RX_REASON_TUNNEL_OBJECT_VALIDATION_FAIL BCMPKT_RX_REASON_TUNNEL_OBJECT_VALIDATION_FAIL /* Bit31 - tunnel with
                                                          object validation
                                                          check fail */

#define BCM_PKTIO_RX_REASON_CPU_L3CPU   BCMPKT_RX_REASON_CPU_L3CPU /* L3 copy to CPU. */

#define BCM_PKTIO_RX_REASON_VNTAG_UNKNOWN_SUBTENDING_PORT_ERROR BCMPKT_RX_REASON_VNTAG_UNKNOWN_SUBTENDING_PORT_ERROR /* VNTAG unknown
                                                          SUBTENDING port */

#define BCM_PKTIO_RX_REASON_RPF_MISS    BCMPKT_RX_REASON_RPF_MISS /* RPF Lookup Miss */

#define BCM_PKTIO_RX_REASON_DOT1P_ADMITTANCE_DISCARD BCMPKT_RX_REASON_DOT1P_ADMITTANCE_DISCARD /* Dot1p Admittance
                                                          Discard */

#define BCM_PKTIO_RX_REASON_TUNNEL_ADAPT_LOOKUP_MISS_DROP BCMPKT_RX_REASON_TUNNEL_ADAPT_LOOKUP_MISS_DROP /* It should be renamed
                                                          to
                                                          TUNNEL_ADAPT_LOOKUP_CPU.
                                                          Indicates packet was
                                                          copiedto the CPU due
                                                          to
                                                          TUNNEL_ADAPT_LOOKUP's
                                                          miss policy or hit
                                                          policy copy to cpu. */

#define BCM_PKTIO_RX_REASON_PKT_FLOW_SELECT_MISS BCMPKT_RX_REASON_PKT_FLOW_SELECT_MISS /* PKT_FLOW_SELECT_MISS_TO_CPU */

#define BCM_PKTIO_RX_REASON_PKT_FLOW_SELECT BCMPKT_RX_REASON_PKT_FLOW_SELECT /* Indicates packet was
                                                          copied to the CPU due
                                                          to PKT_FLOW_SELECT
                                                          policy. */

#define BCM_PKTIO_RX_REASON_CPU_FORWARDING_OTHER BCMPKT_RX_REASON_CPU_FORWARDING_OTHER /* It should be renamed
                                                          to
                                                          TUNNEL_ADAPT_LOOKUP_MISS_TO_CPU_FROM_LOGICAL_TABLE_SEL.Indicates
                                                          packet was copied to
                                                          the CPU due to
                                                          FORWARDING_LOOKUP
                                                          miss. */

#define BCM_PKTIO_RX_REASON_INVALID_TPID    BCMPKT_RX_REASON_INVALID_TPID 

#define BCM_PKTIO_RX_REASON_MPLS_CONTROL_PKT BCMPKT_RX_REASON_MPLS_CONTROL_PKT 

#define BCM_PKTIO_RX_REASON_TUNNEL_ERR  BCMPKT_RX_REASON_TUNNEL_ERR /* TUNNEL_ERR_TO_CPU */

#define BCM_PKTIO_RX_REASON_TUNNEL_TTL_ERR  BCMPKT_RX_REASON_TUNNEL_TTL_ERR /* TUNNEL_TTL_ERR_TO_CPU 
                                                          Error */

#define BCM_PKTIO_RX_REASON_L3_HDR_ERROR    BCMPKT_RX_REASON_L3_HDR_ERROR 

#define BCM_PKTIO_RX_REASON_L2_HDR_ERROR    BCMPKT_RX_REASON_L2_HDR_ERROR 

#define BCM_PKTIO_RX_REASON_TTL1_ERR    BCMPKT_RX_REASON_TTL1_ERR 

#define BCM_PKTIO_RX_REASON_TTL_ERR BCMPKT_RX_REASON_TTL_ERR 

#define BCM_PKTIO_RX_REASON_NAT_ERROR   BCMPKT_RX_REASON_NAT_ERROR /* Nat error packet. */

#define BCM_PKTIO_RX_REASON_L2_MTU_CHECK_FAIL BCMPKT_RX_REASON_L2_MTU_CHECK_FAIL /* L2 MTU check fail to
                                                          CPU */

#define BCM_PKTIO_RX_REASON_L2_MAC_LIMIT    BCMPKT_RX_REASON_L2_MAC_LIMIT /* Over system MAC limit
                                                          threshold to CPU */

#define BCM_PKTIO_RX_REASON_L2_STU_CHECK_FAIL BCMPKT_RX_REASON_L2_STU_CHECK_FAIL /* L2 STU check fail to
                                                          CPU */

#define BCM_PKTIO_RX_REASON_SR_COUNTER_LIMIT BCMPKT_RX_REASON_SR_COUNTER_LIMIT /* SR counter threshold
                                                          exceeded to CPU. */

#define BCM_PKTIO_RX_REASON_SRV6_ERROR  BCMPKT_RX_REASON_SRV6_ERROR /* SRV6_PROC_ERROR */

#define BCM_PKTIO_RX_REASON_COUNT   BCMPKT_RX_REASON_COUNT /* RX REASON TYPE NUMBER */

#define BCM_PKTIO_TXPMD_FID_INVALID BCMPKT_TXPMD_FID_INVALID /* Invalid BCM_PKTIO_TXPMD FID
                                                  indicator */

#define BCM_PKTIO_TXPMD_START   BCMPKT_TXPMD_START /* Start of frame indicator. */

#define BCM_PKTIO_TXPMD_HEADER_TYPE BCMPKT_TXPMD_HEADER_TYPE /* 64 Header Types Supported. 1
                                                  means Packets from CPU with
                                                  SOBMH header formatto Passthru
                                                  NLF */

#define BCM_PKTIO_TXPMD_PKT_LENGTH  BCMPKT_TXPMD_PKT_LENGTH /* PKT_LENGTH (set by hardware -
                                                  in Iarb). */

#define BCM_PKTIO_TXPMD_IPCF_PTR    BCMPKT_TXPMD_IPCF_PTR /* Pointer to cell buffer of this
                                                  SOBMH cell. (Set by hardware) */

#define BCM_PKTIO_TXPMD_SOP     BCMPKT_TXPMD_SOP /* SOP indicator (set by hardware -
                                              in Iarb). */

#define BCM_PKTIO_TXPMD_EOP     BCMPKT_TXPMD_EOP /* EOP indicator (set by hardware -
                                              in Iarb). */

#define BCM_PKTIO_TXPMD_CELL_LENGTH BCMPKT_TXPMD_CELL_LENGTH /* CELL_LENGTH (set by hardware -
                                                  in Iarb). */

#define BCM_PKTIO_TXPMD_CELL_ERROR  BCMPKT_TXPMD_CELL_ERROR /* CELL_ERROR or PURGE */

#define BCM_PKTIO_TXPMD_LOCAL_DEST_PORT BCMPKT_TXPMD_LOCAL_DEST_PORT /* Indicates the local port
                                                      to send a SOBMH packet
                                                      out. */

#define BCM_PKTIO_TXPMD_SRC_MODID   BCMPKT_TXPMD_SRC_MODID /* Source module ID, must be
                                                  programmed to MY_MODID. */

#define BCM_PKTIO_TXPMD_COS     BCMPKT_TXPMD_COS /* Class of service for MMU queueing
                                              for this packet - sets COS
                                              values,PBI.UC_COS, PBI.MC_COS1,
                                              and PBI.MC_COS2. */

#define BCM_PKTIO_TXPMD_INPUT_PRI   BCMPKT_TXPMD_INPUT_PRI /* Traffic priority to be applied
                                                  to MMU via PBI.INPUT_PRIORITY. */

#define BCM_PKTIO_TXPMD_UNICAST BCMPKT_TXPMD_UNICAST /* Indicates that PBI.UNICAST should
                                              be set to queue as unicast packet. */

#define BCM_PKTIO_TXPMD_RQE_Q_NUM   BCMPKT_TXPMD_RQE_Q_NUM /* Value for CCBI.RSQ_Q_NUM. */

#define BCM_PKTIO_TXPMD_SET_L2BM    BCMPKT_TXPMD_SET_L2BM /* Indicates that PBI.L2_BITMAP
                                                  should be set (to queue as
                                                  L2MC packet). */

#define BCM_PKTIO_TXPMD_IEEE1588_ONE_STEP_ENABLE BCMPKT_TXPMD_IEEE1588_ONE_STEP_ENABLE /* ONE STEP TIME STAMPING
                                                          ENABLE */

#define BCM_PKTIO_TXPMD_IEEE1588_REGEN_UDP_CHECKSUM BCMPKT_TXPMD_IEEE1588_REGEN_UDP_CHECKSUM /* Regenerate UDP
                                                          Checksum */

#define BCM_PKTIO_TXPMD_IEEE1588_INGRESS_TIMESTAMP_SIGN BCMPKT_TXPMD_IEEE1588_INGRESS_TIMESTAMP_SIGN /* ITS_SIGN bit */

#define BCM_PKTIO_TXPMD_IEEE1588_TIMESTAMP_HDR_OFFSET BCMPKT_TXPMD_IEEE1588_TIMESTAMP_HDR_OFFSET /* HDR_OFFSET */

#define BCM_PKTIO_TXPMD_TX_TS   BCMPKT_TXPMD_TX_TS /* Indicates for TS packet
                                              transmitted from CPU into IP that
                                              the outgoing packetneeds to have
                                              its transmit timestamp captured by
                                              the port. */

#define BCM_PKTIO_TXPMD_SPID_OVERRIDE   BCMPKT_TXPMD_SPID_OVERRIDE /* For PBI.SPID_Override */

#define BCM_PKTIO_TXPMD_SPID    BCMPKT_TXPMD_SPID /* For PBI.SPID - Service Pool ID */

#define BCM_PKTIO_TXPMD_SPAP    BCMPKT_TXPMD_SPAP /* For PBI.SPAP - Service Pool
                                              Priority (color) */

#define BCM_PKTIO_TXPMD_QUEUE_NUM   BCMPKT_TXPMD_QUEUE_NUM /* Queue number to be used for
                                                  unicast queuing and CPU queue
                                                  (CPU_COS). */

#define BCM_PKTIO_TXPMD_SET_L3BM    BCMPKT_TXPMD_SET_L3BM /* Indicates that PBI.L3_BITMAP
                                                  should be set (to queue as
                                                  IPMC packet). */

#define BCM_PKTIO_TXPMD_EP_CPU_REASON_CODE  BCMPKT_TXPMD_EP_CPU_REASON_CODE /* EP Copy to CPU Reasons */

#define BCM_PKTIO_TXPMD_UPMEP_TX_FROM_HG_PORT BCMPKT_TXPMD_UPMEP_TX_FROM_HG_PORT /* Indicates the UPMEP TX
                                                          packet is from higig
                                                          port. This information
                                                          needsto be brought
                                                          through loopback
                                                          header to the 2nd pass
                                                          so that Ep know
                                                          howmany bytes to
                                                          decap. */

#define BCM_PKTIO_TXPMD_UPMEP_TX_LOOPBACK   BCMPKT_TXPMD_UPMEP_TX_LOOPBACK /* Indicates the packet
                                                          is UPMEP TX loopback
                                                          1st pass. */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_COUNTER_1_ACTION BCMPKT_TXPMD_OAM_UPMEP_TX_COUNTER_1_ACTION /* Selects action for
                                                          updating or sampling
                                                          LM count for LM
                                                          counter pool 0 */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_COUNTER_1_ID BCMPKT_TXPMD_OAM_UPMEP_TX_COUNTER_1_ID /* Counter index for LM
                                                          counter pool 0 */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_COUNTER_1_LOCATION BCMPKT_TXPMD_OAM_UPMEP_TX_COUNTER_1_LOCATION /* OAM upstream counter 1
                                                          location. */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_COUNTER_2_ACTION BCMPKT_TXPMD_OAM_UPMEP_TX_COUNTER_2_ACTION /* Selects action for
                                                          updating or sampling
                                                          LM count for LM
                                                          counter pool 1 */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_COUNTER_2_ID BCMPKT_TXPMD_OAM_UPMEP_TX_COUNTER_2_ID /* Counter index for LM
                                                          counter pool 1 */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_COUNTER_2_LOCATION BCMPKT_TXPMD_OAM_UPMEP_TX_COUNTER_2_LOCATION /* OAM upstream counter 2
                                                          location. */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_INT_PRI BCMPKT_TXPMD_OAM_UPMEP_TX_INT_PRI /* Valid for CPU/HW
                                                          Gen/OLP Packets. For
                                                          OLP Packets populate
                                                          from OLP_TX.INT_PRI */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_OAM_REPLACEMENT_OFFSET BCMPKT_TXPMD_OAM_UPMEP_TX_OAM_REPLACEMENT_OFFSET /* Offset into packet
                                                          where
                                                          counter/timestamp must
                                                          be inserted. Value is
                                                          in2-byteincrements
                                                          starting from 14 bytes
                                                          after end of OLP TX
                                                          header */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_OLP_PKT_FROM_HG_PORT BCMPKT_TXPMD_OAM_UPMEP_TX_OLP_PKT_FROM_HG_PORT /* For OLP Packets - Set
                                                          if the packet is
                                                          received from Remote
                                                          OLP Port. Thisis used
                                                          by EP to NOT decap OLP
                                                          vlan tag as it is
                                                          present in the HG
                                                          header. */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_SGPP   BCMPKT_TXPMD_OAM_UPMEP_TX_SGPP /* Taken from the OLP
                                                          header. Packet is
                                                          processed as if it
                                                          came in on the SGPPin
                                                          this field. */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_SOURCE_OLP BCMPKT_TXPMD_OAM_UPMEP_TX_SOURCE_OLP /* Indicates that OLP
                                                          header is present in
                                                          the apcket. Triggers a
                                                          decap of OLPheader
                                                          before packet is
                                                          transmitted */

#define BCM_PKTIO_TXPMD_OAM_UPMEP_TX_TIMESTAMP_ACTION BCMPKT_TXPMD_OAM_UPMEP_TX_TIMESTAMP_ACTION /* Enables timestamp
                                                          sampling and selects
                                                          between NTP and 1588
                                                          format timestamps */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_COUNTER_1_ACTION BCMPKT_TXPMD_OAM_DOWNMEP_TX_COUNTER_1_ACTION /* Selects action for
                                                          updating or sampling
                                                          LM count for LM
                                                          counter pool 0 */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_COUNTER_1_ID BCMPKT_TXPMD_OAM_DOWNMEP_TX_COUNTER_1_ID /* Counter index for LM
                                                          counter pool 0 */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_COUNTER_1_LOCATION BCMPKT_TXPMD_OAM_DOWNMEP_TX_COUNTER_1_LOCATION /* OAM downstream counter
                                                          1 location. */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_COUNTER_2_ACTION BCMPKT_TXPMD_OAM_DOWNMEP_TX_COUNTER_2_ACTION /* Selects action for
                                                          updating or sampling
                                                          LM count for LM
                                                          counter pool 1 */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_COUNTER_2_ID BCMPKT_TXPMD_OAM_DOWNMEP_TX_COUNTER_2_ID /* Counter index for LM
                                                          counter pool 1 */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_COUNTER_2_LOCATION BCMPKT_TXPMD_OAM_DOWNMEP_TX_COUNTER_2_LOCATION /* OAM downstream counter
                                                          2 location. */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_DGPP BCMPKT_TXPMD_OAM_DOWNMEP_TX_DGPP /* Destination global
                                                          physical port */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_INPUT_PRIORITY BCMPKT_TXPMD_OAM_DOWNMEP_TX_INPUT_PRIORITY /* Traffic Priority to be
                                                          applied to MMU. For
                                                          OLP packets it will be
                                                          from HG2HDR(if OLP
                                                          packet received from
                                                          HG port) or
                                                          OLP_VLAN.PRI (if OLP
                                                          packet receivedfrom
                                                          front-panel port). */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_INT_PRI BCMPKT_TXPMD_OAM_DOWNMEP_TX_INT_PRI /* Used for OLP Packets -
                                                          OLP_TX.INT_PRI */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_OAM_REPLACEMENT_OFFSET BCMPKT_TXPMD_OAM_DOWNMEP_TX_OAM_REPLACEMENT_OFFSET /* Offset into packet
                                                          where
                                                          counter/timestamp must
                                                          be inserted. Value is
                                                          in 2-byteincrements
                                                          starting from 14 bytes
                                                          after end of OLP TX
                                                          header */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_OLP_PKT_FROM_HG_PORT BCMPKT_TXPMD_OAM_DOWNMEP_TX_OLP_PKT_FROM_HG_PORT /* For OLP Packets - Set
                                                          if the packet is
                                                          received from Remote
                                                          OLP Port. This isused
                                                          by EP to NOT decap OLP
                                                          vlan tag as it is
                                                          present in the HG
                                                          header. */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_SOURCE_OLP BCMPKT_TXPMD_OAM_DOWNMEP_TX_SOURCE_OLP /* For OLP Packets - Set
                                                          if the packet is
                                                          received from Local or
                                                          Remote OLP port.It is
                                                          for the pipeline to
                                                          distinguish between
                                                          CPU/HW generated vs
                                                          OLP */

#define BCM_PKTIO_TXPMD_OAM_DOWNMEP_TX_TIMESTAMP_ACTION BCMPKT_TXPMD_OAM_DOWNMEP_TX_TIMESTAMP_ACTION /* Enables timestamp
                                                          sampling and selects
                                                          between NTP and 1588
                                                          format timestamps */

#define BCM_PKTIO_TXPMD_NLF_PORT_NUMBER BCMPKT_TXPMD_NLF_PORT_NUMBER /* Logical Egress port number
                                                      provided by the EP */

#define BCM_PKTIO_TXPMD_INT_PRI BCMPKT_TXPMD_INT_PRI /* Internal Priority used for
                                              CPU_COS_MAP TCAM lookups for EP
                                              COPY TO CPU (header type 0) to
                                              generate CPU_COS */

#define BCM_PKTIO_TXPMD_OAM_REPLACEMENT_TYPE BCMPKT_TXPMD_OAM_REPLACEMENT_TYPE /* 00:NOP 01:DM 10:NTP
                                                          11:LM */

#define BCM_PKTIO_TXPMD_OAM_REPLACEMENT_OFFSET BCMPKT_TXPMD_OAM_REPLACEMENT_OFFSET /* From CPU indicating
                                                          insertion point in
                                                          packet for LM count or
                                                          DM Timestamp. Has 14
                                                          Byte offset with 2
                                                          byte increments */

#define BCM_PKTIO_TXPMD_QUEUE_NUM_UPPER BCMPKT_TXPMD_QUEUE_NUM_UPPER /* Holds upper 2 bit of the
                                                      QUEUE_NUM field. */

#define BCM_PKTIO_TXPMD_LM_COUNTER_INDEX    BCMPKT_TXPMD_LM_COUNTER_INDEX /* From CPU indicating LM
                                                          Counter Index */

#define BCM_PKTIO_TXPMD_UNICAST_PKT BCMPKT_TXPMD_UNICAST_PKT /* Will set CCBI_B.UNICAST_PKT */

#define BCM_PKTIO_TXPMD_FID_COUNT   BCMPKT_TXPMD_FID_COUNT /* TXPMD FIELD ID NUMBER */

#define BCM_PKTIO_TXPMD_START_INTERNAL_HEADER BCMPKT_TXPMD_START_INTERNAL_HEADER /* The header used
                                                          internally only */

#define BCM_PKTIO_TXPMD_START_HIGIG BCMPKT_TXPMD_START_HIGIG /* Frame type is Higig */

#define BCM_PKTIO_TXPMD_HEADER_T_TO_CPU BCMPKT_TXPMD_HEADER_T_TO_CPU /* EP Copy to CPU format,
                                                      SOBMH header in EP to
                                                      Passthru NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_FROM_CPU   BCMPKT_TXPMD_HEADER_T_FROM_CPU /* Packets from CPU with
                                                          SOBMH header format to
                                                          Passthru NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_MIM    BCMPKT_TXPMD_HEADER_T_MIM /* MAC in MAC packets to
                                                      Passthru NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_MPLS_PMP   BCMPKT_TXPMD_HEADER_T_MPLS_PMP /* MPLS packets to
                                                          Passthru NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_TRILL_NW   BCMPKT_TXPMD_HEADER_T_TRILL_NW /* Trill Network Packets
                                                          to Passthru NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_TRILL_AC   BCMPKT_TXPMD_HEADER_T_TRILL_AC /* Trill Access Layer
                                                          Packets to Passthru
                                                          NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_WLAN_DECAP BCMPKT_TXPMD_HEADER_T_WLAN_DECAP /* WLAN Decap packets
                                                          sent to WRX NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_WLAN_ENCAP BCMPKT_TXPMD_HEADER_T_WLAN_ENCAP /* WLAN Encap packets
                                                          sent to WTX NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_QCN    BCMPKT_TXPMD_HEADER_T_QCN /* QCN Packets to Passthru
                                                      NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_SM_DPI BCMPKT_TXPMD_HEADER_T_SM_DPI /* DPI/Signature Matcher
                                                      packets sent to SM NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_EP_REDIR   BCMPKT_TXPMD_HEADER_T_EP_REDIR /* EP Redirection packets
                                                          to Passthru NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_GENERIC    BCMPKT_TXPMD_HEADER_T_GENERIC /* Other generic
                                                          passthrough to
                                                          Passthru NLF */

#define BCM_PKTIO_TXPMD_HEADER_T_OAM_DOWNMEP_TX BCMPKT_TXPMD_HEADER_T_OAM_DOWNMEP_TX /* DOWN MEP Transmit OAM
                                                          Packets from CPU or
                                                          CCM from HW Engine or
                                                          OLP */

#define BCM_PKTIO_TXPMD_HEADER_T_OAM_UPMEP_TX BCMPKT_TXPMD_HEADER_T_OAM_UPMEP_TX /* UP MEP Transmit OAM
                                                          Packets from CPU or
                                                          CCM from HW Engine or
                                                          OLP */

#define BCM_PKTIO_TRACE_FID_INVALID BCMPKT_TRACE_FID_INVALID /* Invalid BCM_PKTIO_TRACE FID
                                                  indicator */

#define BCM_PKTIO_TRACE_MY_STATION_HIT  BCMPKT_TRACE_MY_STATION_HIT /* Indicates MY_STATION_TCAM
                                                      lookup hit. */

#define BCM_PKTIO_TRACE_FWD_VLAN    BCMPKT_TRACE_FWD_VLAN /* Forward VLAN */

#define BCM_PKTIO_TRACE_FP_AND_PBM_ACTION_VALID BCMPKT_TRACE_FP_AND_PBM_ACTION_VALID /* FP and PBM action */

#define BCM_PKTIO_TRACE_FP_REPLACE_PBM_ACTION_VALID BCMPKT_TRACE_FP_REPLACE_PBM_ACTION_VALID /* FP replace PBM action. */

#define BCM_PKTIO_TRACE_FP_OR_PBM_ACTION_VALID BCMPKT_TRACE_FP_OR_PBM_ACTION_VALID /* FP or PBM Action */

#define BCM_PKTIO_TRACE_INNER_PAYLOAD_TAG_STATUS BCMPKT_TRACE_INNER_PAYLOAD_TAG_STATUS /* Inner payload TAG
                                                          status. */

#define BCM_PKTIO_TRACE_POST_VXLT_TAG_STATUS BCMPKT_TRACE_POST_VXLT_TAG_STATUS /* Post VLAN Translation
                                                          TAG status. */

#define BCM_PKTIO_TRACE_INCOMING_TAG_STATUS BCMPKT_TRACE_INCOMING_TAG_STATUS /* Incoming TAG Status. */

#define BCM_PKTIO_TRACE_FORWARDING_FIELD    BCMPKT_TRACE_FORWARDING_FIELD /* Forwarding Field. */

#define BCM_PKTIO_TRACE_FORWARDING_TYPE BCMPKT_TRACE_FORWARDING_TYPE /* Forwarding Type. */

#define BCM_PKTIO_TRACE_EGRESS_MASK_TABLE_INDEX BCMPKT_TRACE_EGRESS_MASK_TABLE_INDEX /* Egress Mask Table
                                                          index. */

#define BCM_PKTIO_TRACE_EGRESS_BLOCK_MASK_INDEX BCMPKT_TRACE_EGRESS_BLOCK_MASK_INDEX /* Egress Block Mask
                                                          index. */

#define BCM_PKTIO_TRACE_VLAN_MASK_INDEX BCMPKT_TRACE_VLAN_MASK_INDEX /* VLAN Mask index. */

#define BCM_PKTIO_TRACE_MAC_BLOCK_INDEX BCMPKT_TRACE_MAC_BLOCK_INDEX /* MAC Block index. */

#define BCM_PKTIO_TRACE_NONUC_TRUNK_BLOCK_MASK_INDEX BCMPKT_TRACE_NONUC_TRUNK_BLOCK_MASK_INDEX /* Non Unicast Trunk
                                                          Block Mask index */

#define BCM_PKTIO_TRACE_HASH_VALUE_HG   BCMPKT_TRACE_HASH_VALUE_HG /* HiGig Hash Value. */

#define BCM_PKTIO_TRACE_HG_OFFSET   BCMPKT_TRACE_HG_OFFSET /* HiGig Offset. */

#define BCM_PKTIO_TRACE_HG_PORT BCMPKT_TRACE_HG_PORT /* HiGig Port. */

#define BCM_PKTIO_TRACE_HG_TRUNK_ID BCMPKT_TRACE_HG_TRUNK_ID /* HiGig Trunk ID. */

#define BCM_PKTIO_TRACE_HG_TRUNK_RESOLUTION_DONE BCMPKT_TRACE_HG_TRUNK_RESOLUTION_DONE /* HiGig Trunk resolution
                                                          done. */

#define BCM_PKTIO_TRACE_DGPP    BCMPKT_TRACE_DGPP /* Destination GPP. */

#define BCM_PKTIO_TRACE_NHOP    BCMPKT_TRACE_NHOP /* Next Hop. */

#define BCM_PKTIO_TRACE_LAG_OFFSET  BCMPKT_TRACE_LAG_OFFSET /* LAG Offset. */

#define BCM_PKTIO_TRACE_HASH_VALUE_LAG  BCMPKT_TRACE_HASH_VALUE_LAG /* LAG Hash Value. */

#define BCM_PKTIO_TRACE_LAG_ID  BCMPKT_TRACE_LAG_ID /* LAG ID. */

#define BCM_PKTIO_TRACE_LAG_RESOLUTION_DONE BCMPKT_TRACE_LAG_RESOLUTION_DONE /* LAG Resolution Done. */

#define BCM_PKTIO_TRACE_RESOLVED_NETWORK_DVP BCMPKT_TRACE_RESOLVED_NETWORK_DVP /* Resolved Network DVP. */

#define BCM_PKTIO_TRACE_NETWORK_VP_LAG_VALID BCMPKT_TRACE_NETWORK_VP_LAG_VALID /* Network VP LAG Valid. */

#define BCM_PKTIO_TRACE_ECMP_HASH_VALUE2    BCMPKT_TRACE_ECMP_HASH_VALUE2 /* ECMP Hash Value 2. */

#define BCM_PKTIO_TRACE_ECMP_OFFSET2    BCMPKT_TRACE_ECMP_OFFSET2 /* ECMP Offset 2. */

#define BCM_PKTIO_TRACE_ECMP_GROUP2 BCMPKT_TRACE_ECMP_GROUP2 /* ECMP Group 2. */

#define BCM_PKTIO_TRACE_ECMP_RESOLUTION_DONE2 BCMPKT_TRACE_ECMP_RESOLUTION_DONE2 /* ECMP Resolution Done
                                                          2. */

#define BCM_PKTIO_TRACE_ECMP_HASH_VALUE1    BCMPKT_TRACE_ECMP_HASH_VALUE1 /* ECMP Hash Value 1. */

#define BCM_PKTIO_TRACE_ECMP_OFFSET1    BCMPKT_TRACE_ECMP_OFFSET1 /* ECMP Offset 1. */

#define BCM_PKTIO_TRACE_ECMP_GROUP1 BCMPKT_TRACE_ECMP_GROUP1 /* ECMP Group 1. */

#define BCM_PKTIO_TRACE_ECMP_RESOLUTION_DONE1 BCMPKT_TRACE_ECMP_RESOLUTION_DONE1 /* ECMP Resolution Done
                                                          1. */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_FIELD BCMPKT_TRACE_FWD_DESTINATION_FIELD /* Reserved. */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_TYPE BCMPKT_TRACE_FWD_DESTINATION_TYPE /* Forward Destination
                                                          Type. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR BCMPKT_TRACE_PKT_RESOLUTION_VECTOR /* Packet Resolution
                                                          vector. */

#define BCM_PKTIO_TRACE_VXLT_HIT    BCMPKT_TRACE_VXLT_HIT /* VXLT hit. */

#define BCM_PKTIO_TRACE_VALID_VLAN_ID   BCMPKT_TRACE_VALID_VLAN_ID /* Valid vlan ID. */

#define BCM_PKTIO_TRACE_INGRESS_STG_STATE   BCMPKT_TRACE_INGRESS_STG_STATE /* Ingress stg state. */

#define BCM_PKTIO_TRACE_L2_SRC_HIT  BCMPKT_TRACE_L2_SRC_HIT /* L2 source hit. */

#define BCM_PKTIO_TRACE_L2_SRC_STATIC   BCMPKT_TRACE_L2_SRC_STATIC /* L2 source static. */

#define BCM_PKTIO_TRACE_L2_DST_HIT  BCMPKT_TRACE_L2_DST_HIT /* L2 destination hit. */

#define BCM_PKTIO_TRACE_L2_USER_ENTRY_HIT   BCMPKT_TRACE_L2_USER_ENTRY_HIT /* L2 user entry hit. */

#define BCM_PKTIO_TRACE_L3_ENTRY_SOURCE_HIT BCMPKT_TRACE_L3_ENTRY_SOURCE_HIT /* L3 entry source hit. */

#define BCM_PKTIO_TRACE_L3_ENTRY_DESTINATION_HIT BCMPKT_TRACE_L3_ENTRY_DESTINATION_HIT /* L3 entry destination
                                                          hit. */

#define BCM_PKTIO_TRACE_LPM_HIT BCMPKT_TRACE_LPM_HIT /* LPM hit. */

#define BCM_PKTIO_TRACE_UNRESOLVED_SA   BCMPKT_TRACE_UNRESOLVED_SA /* Unresolved source address. */

#define BCM_PKTIO_TRACE_DOS_ATTACK  BCMPKT_TRACE_DOS_ATTACK /* Dos attack. */

#define BCM_PKTIO_TRACE_L3_TUNNEL_HIT   BCMPKT_TRACE_L3_TUNNEL_HIT /* L3 tunnel hit. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_TABLE_LOOKUP_0_HIT BCMPKT_TRACE_MPLS_ENTRY_TABLE_LOOKUP_0_HIT /* MPLS entry table
                                                          lookup 0 hit. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_TABLE_LOOKUP_1_HIT BCMPKT_TRACE_MPLS_ENTRY_TABLE_LOOKUP_1_HIT /* MPLS entry table
                                                          lookup 1 hit. */

#define BCM_PKTIO_TRACE_MPLS_BOS_TERMINATED BCMPKT_TRACE_MPLS_BOS_TERMINATED /* MPLS bos terminated. */

#define BCM_PKTIO_TRACE_SRC_TBL_HALF_ENTRY_NUM BCMPKT_TRACE_SRC_TBL_HALF_ENTRY_NUM /* LPM table source
                                                          address half entry
                                                          number. */

#define BCM_PKTIO_TRACE_SRC_TBL_INDEX   BCMPKT_TRACE_SRC_TBL_INDEX /* LPM table source address
                                                      table index */

#define BCM_PKTIO_TRACE_TBL_HALF_ENTRY_NUM  BCMPKT_TRACE_TBL_HALF_ENTRY_NUM /* LPM table destination
                                                          address half entry
                                                          number. */

#define BCM_PKTIO_TRACE_TBL_INDEX   BCMPKT_TRACE_TBL_INDEX /* LPM table destination address
                                                  table index. */

#define BCM_PKTIO_TRACE_DEF64_SRC_PUBLIC_HIT BCMPKT_TRACE_DEF64_SRC_PUBLIC_HIT /* DEF64 source address
                                                          public hit. */

#define BCM_PKTIO_TRACE_DEF64_SRC_PRIVATE_HIT BCMPKT_TRACE_DEF64_SRC_PRIVATE_HIT /* DEF64 source address
                                                          private hit. */

#define BCM_PKTIO_TRACE_DEF64_PUBLIC_HIT    BCMPKT_TRACE_DEF64_PUBLIC_HIT /* DEF64 destination
                                                          address public hit. */

#define BCM_PKTIO_TRACE_DEF64_PRIVATE_HIT   BCMPKT_TRACE_DEF64_PRIVATE_HIT /* DEF64 destination
                                                          address private hit. */

#define BCM_PKTIO_TRACE_DEF128_SRC_PUBLIC_HIT BCMPKT_TRACE_DEF128_SRC_PUBLIC_HIT /* DEF128 source address
                                                          public hit. */

#define BCM_PKTIO_TRACE_DEF128_SRC_PRIVATE_HIT BCMPKT_TRACE_DEF128_SRC_PRIVATE_HIT /* DEF128 source address
                                                          private hit. */

#define BCM_PKTIO_TRACE_DEF128_PUBLIC_HIT   BCMPKT_TRACE_DEF128_PUBLIC_HIT /* DEF128 destination
                                                          address public hit. */

#define BCM_PKTIO_TRACE_DEF128_PRIVATE_HIT  BCMPKT_TRACE_DEF128_PRIVATE_HIT /* DEF128 destination
                                                          address private hit. */

#define BCM_PKTIO_TRACE_L3_LKUP2_KEY_TYPE   BCMPKT_TRACE_L3_LKUP2_KEY_TYPE /* L3 lookup 2 key type */

#define BCM_PKTIO_TRACE_L3_LKUP1_KEY_TYPE   BCMPKT_TRACE_L3_LKUP1_KEY_TYPE /* L3 lookup 1 key type. */

#define BCM_PKTIO_TRACE_SRC_L3_ENTRY_BITMAP BCMPKT_TRACE_SRC_L3_ENTRY_BITMAP /* L3 lookup source
                                                          address entry bitmap. */

#define BCM_PKTIO_TRACE_SRC_L3_BUCKET_INDEX BCMPKT_TRACE_SRC_L3_BUCKET_INDEX /* L3 lookup source
                                                          address bucket index. */

#define BCM_PKTIO_TRACE_SRC_PRIVATE_HIT BCMPKT_TRACE_SRC_PRIVATE_HIT /* L3 lookup source address
                                                      private hit. */

#define BCM_PKTIO_TRACE_SRC_PUBLIC_HIT  BCMPKT_TRACE_SRC_PUBLIC_HIT /* L3 lookup source address
                                                      public hit. */

#define BCM_PKTIO_TRACE_DST_L3_ENTRY_BITMAP BCMPKT_TRACE_DST_L3_ENTRY_BITMAP /* L3 lookup destination
                                                          address L3 entry
                                                          bitmap. */

#define BCM_PKTIO_TRACE_DST_L3_BUCKET_INDEX BCMPKT_TRACE_DST_L3_BUCKET_INDEX /* L3 lookup destination
                                                          address bucket index. */

#define BCM_PKTIO_TRACE_DST_PRIVATE_HIT BCMPKT_TRACE_DST_PRIVATE_HIT /* L3 lookup destination
                                                      address private hit. */

#define BCM_PKTIO_TRACE_DST_PUBLIC_HIT  BCMPKT_TRACE_DST_PUBLIC_HIT /* L3 lookup destination
                                                      address public hit. */

#define BCM_PKTIO_TRACE_L2LU_SRC_INDEX  BCMPKT_TRACE_L2LU_SRC_INDEX /* L2 lookup source index. */

#define BCM_PKTIO_TRACE_L2LU_SRC_KEY_TYPE   BCMPKT_TRACE_L2LU_SRC_KEY_TYPE /* Key type used for
                                                          Source lookup to L2
                                                          table. */

#define BCM_PKTIO_TRACE_L2LU_SRC_HIT    BCMPKT_TRACE_L2LU_SRC_HIT /* Indicates Source lookup in
                                                      L2 table hit. */

#define BCM_PKTIO_TRACE_L2LU_DST_IDX    BCMPKT_TRACE_L2LU_DST_IDX /* Resulting Index when
                                                      destination lookup in L2
                                                      table hit. */

#define BCM_PKTIO_TRACE_L2LU_DST_KEY_TYPE   BCMPKT_TRACE_L2LU_DST_KEY_TYPE /* Key type used for
                                                          Destination lookup to
                                                          L2 table. */

#define BCM_PKTIO_TRACE_L2LU_DST_HIT    BCMPKT_TRACE_L2LU_DST_HIT /* Indicates Destination
                                                      lookup in L2 table hit. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_INDEX_2  BCMPKT_TRACE_MPLS_ENTRY_INDEX_2 /* Index when MPLS_ENTRY
                                                          lookup 2 hit. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_KEY_TYPE_2 BCMPKT_TRACE_MPLS_ENTRY_KEY_TYPE_2 /* Key type used for
                                                          MPLS_ENTRY lookup 2. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_INDEX_1  BCMPKT_TRACE_MPLS_ENTRY_INDEX_1 /* Index when MPLS_ENTRY
                                                          lookup 1 hit. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_KEY_TYPE_1 BCMPKT_TRACE_MPLS_ENTRY_KEY_TYPE_1 /* Key type used for
                                                          MPLS_ENTRY lookup 1. */

#define BCM_PKTIO_TRACE_VT_INDEX_2  BCMPKT_TRACE_VT_INDEX_2 /* Index for VLAN translation
                                                  lookup 2 when hit. */

#define BCM_PKTIO_TRACE_VT_INDEX_1  BCMPKT_TRACE_VT_INDEX_1 /* Index for VLAN translation
                                                  lookup 1 when hit. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_HIT_1    BCMPKT_TRACE_MPLS_ENTRY_HIT_1 /* Indicates MPLS_ENTRY
                                                          lookup 1 hit. */

#define BCM_PKTIO_TRACE_MPLS_ENTRY_HIT_2    BCMPKT_TRACE_MPLS_ENTRY_HIT_2 /* Indicates MPLS_ENTRY
                                                          lookup 2 hit. */

#define BCM_PKTIO_TRACE_VT_KEY_TYPE_2   BCMPKT_TRACE_VT_KEY_TYPE_2 /* Key type used for VLAN
                                                      translation lookup 2. */

#define BCM_PKTIO_TRACE_VT_HIT_2    BCMPKT_TRACE_VT_HIT_2 /* Indicates VLAN translation
                                                  lookup 2 hit. */

#define BCM_PKTIO_TRACE_VT_KEY_TYPE_1   BCMPKT_TRACE_VT_KEY_TYPE_1 /* Key type used for VLAN
                                                      translation lookup 1. */

#define BCM_PKTIO_TRACE_VT_HIT_1    BCMPKT_TRACE_VT_HIT_1 /* Indicates VLAN translation
                                                  lookup 1 hit. */

#define BCM_PKTIO_TRACE_MY_STATION_INDEX    BCMPKT_TRACE_MY_STATION_INDEX /* Index for
                                                          MY_STATION_TCAM when
                                                          hit. */

#define BCM_PKTIO_TRACE_ENTROPY BCMPKT_TRACE_ENTROPY /* Entropy. */

#define BCM_PKTIO_TRACE_HIT_BIT_INDEX   BCMPKT_TRACE_HIT_BIT_INDEX /* Index of hitbit in the hit
                                                      table. [15:0] is the index
                                                      to the hit table, and
                                                      [19:16] is thebit position
                                                      within the entry. */

#define BCM_PKTIO_TRACE_HIT_BIT_TABLE   BCMPKT_TRACE_HIT_BIT_TABLE /* 0: L3_DEFIP_LEVEL1_HIT, 1:
                                                      L3_DEFIP_LEVEL2_HIT, 2:
                                                      L3_DEFIP_LEVEL3_HIT. */

#define BCM_PKTIO_TRACE_SUB_DB_PRIORITY BCMPKT_TRACE_SUB_DB_PRIORITY /* Sub DB Priority, used by
                                                      Level1 resolution block. */

#define BCM_PKTIO_TRACE_L3_ENTRY_DST_HIT    BCMPKT_TRACE_L3_ENTRY_DST_HIT /* L3 lookup destination
                                                          address destination
                                                          hit. */

#define BCM_PKTIO_TRACE_L3_ENTRY_SRC_HIT    BCMPKT_TRACE_L3_ENTRY_SRC_HIT /* L3 lookup destination
                                                          address source hit. */

#define BCM_PKTIO_TRACE_L3_TUNNEL_HIT_1 BCMPKT_TRACE_L3_TUNNEL_HIT_1 /* Indicates L3_TUNNEL lookup
                                                      hit. */

#define BCM_PKTIO_TRACE_L3_TUNNEL_INDEX_1   BCMPKT_TRACE_L3_TUNNEL_INDEX_1 /* Index when L3_TUNNEL
                                                          lookup hit. */

#define BCM_PKTIO_TRACE_L3_TUNNEL_KEY_TYPE_1 BCMPKT_TRACE_L3_TUNNEL_KEY_TYPE_1 /* Key type of L3_TUNNEL
                                                          lookup hit. */

#define BCM_PKTIO_TRACE_COMP_DST_HIT    BCMPKT_TRACE_COMP_DST_HIT /* Indicates LPM table hit. */

#define BCM_PKTIO_TRACE_COMP_DST_HIT_INDEX  BCMPKT_TRACE_COMP_DST_HIT_INDEX /* Index of hitbit in the
                                                          hit table. */

#define BCM_PKTIO_TRACE_COMP_DST_HIT_TABLE  BCMPKT_TRACE_COMP_DST_HIT_TABLE /* 0:
                                                          L3_DEFIP_LEVEL1_HIT,
                                                          1:
                                                          L3_DEFIP_LEVEL2_HIT,
                                                          2:
                                                          L3_DEFIP_LEVEL3_HIT. */

#define BCM_PKTIO_TRACE_COMP_DST_SUB_DB_PRIORITY BCMPKT_TRACE_COMP_DST_SUB_DB_PRIORITY /* Sub DB Priority, used
                                                          by Level1 resolution
                                                          block. */

#define BCM_PKTIO_TRACE_COMP_SRC_HIT    BCMPKT_TRACE_COMP_SRC_HIT /* Indicates LPM table hit. */

#define BCM_PKTIO_TRACE_COMP_SRC_HIT_INDEX  BCMPKT_TRACE_COMP_SRC_HIT_INDEX /* Index of hitbit in the
                                                          hit table. */

#define BCM_PKTIO_TRACE_COMP_SRC_HIT_TABLE  BCMPKT_TRACE_COMP_SRC_HIT_TABLE /* 0:
                                                          L3_DEFIP_LEVEL1_HIT,
                                                          1:
                                                          L3_DEFIP_LEVEL2_HIT,
                                                          2:
                                                          L3_DEFIP_LEVEL3_HIT. */

#define BCM_PKTIO_TRACE_COMP_SRC_SUB_DB_PRIORITY BCMPKT_TRACE_COMP_SRC_SUB_DB_PRIORITY /* Sub DB Priority, used
                                                          by Level1 resolution
                                                          block. */

#define BCM_PKTIO_TRACE_LPM_DST_HIT BCMPKT_TRACE_LPM_DST_HIT /* Indicates LPM table hit. */

#define BCM_PKTIO_TRACE_LPM_DST_HIT_INDEX   BCMPKT_TRACE_LPM_DST_HIT_INDEX /* Index of hitbit in the
                                                          hit table. */

#define BCM_PKTIO_TRACE_LPM_DST_HIT_TABLE   BCMPKT_TRACE_LPM_DST_HIT_TABLE /* 0:
                                                          L3_DEFIP_LEVEL1_HIT.1:
                                                          L3_DEFIP_LEVEL2_HIT.2:
                                                          L3_DEFIP_LEVEL3_HIT. */

#define BCM_PKTIO_TRACE_LPM_DST_SUB_DB_PRIORITY BCMPKT_TRACE_LPM_DST_SUB_DB_PRIORITY /* Sub DB Priority, used
                                                          by Level1 resolution
                                                          block. */

#define BCM_PKTIO_TRACE_LPM_SRC_HIT BCMPKT_TRACE_LPM_SRC_HIT /* Indicates LPM table hit. */

#define BCM_PKTIO_TRACE_LPM_SRC_HIT_INDEX   BCMPKT_TRACE_LPM_SRC_HIT_INDEX /* Index of hitbit in the
                                                          hit table. */

#define BCM_PKTIO_TRACE_LPM_SRC_HIT_TABLE   BCMPKT_TRACE_LPM_SRC_HIT_TABLE /* 0:
                                                          L3_DEFIP_LEVEL1_HIT.1:
                                                          L3_DEFIP_LEVEL2_HIT.2:
                                                          L3_DEFIP_LEVEL3_HIT. */

#define BCM_PKTIO_TRACE_LPM_SRC_SUB_DB_PRIORITY BCMPKT_TRACE_LPM_SRC_SUB_DB_PRIORITY /* Sub DB Priority, used
                                                          by Level1 resolution
                                                          block. */

#define BCM_PKTIO_TRACE_L3_TUNNEL_TCAM_HIT  BCMPKT_TRACE_L3_TUNNEL_TCAM_HIT /* Indicates
                                                          L3_TUNNEL_TcAM lookup
                                                          hit. */

#define BCM_PKTIO_TRACE_L3_TUNNEL_TCAM_INDEX BCMPKT_TRACE_L3_TUNNEL_TCAM_INDEX /* Index when
                                                          L3_TUNNEL_TCAM lookup
                                                          hit. */

#define BCM_PKTIO_TRACE_FID_COUNT   BCMPKT_TRACE_FID_COUNT /* TRACE FIELD ID NUMBER */

#define BCM_PKTIO_TRACE_INCOMING_TAG_STATUS_UNTAGGED BCMPKT_TRACE_INCOMING_TAG_STATUS_UNTAGGED /* Untagged. */

#define BCM_PKTIO_TRACE_INCOMING_TAG_STATUS_TAGGED BCMPKT_TRACE_INCOMING_TAG_STATUS_TAGGED /* Tagged. */

#define BCM_PKTIO_TRACE_FORWARDING_T_VID    BCMPKT_TRACE_FORWARDING_T_VID /* L2 Forwarded based on
                                                          VID. */

#define BCM_PKTIO_TRACE_FORWARDING_T_FID    BCMPKT_TRACE_FORWARDING_T_FID /* L2 Forwarded based on
                                                          Filtering Identifier. */

#define BCM_PKTIO_TRACE_FORWARDING_T_L3_MPLS BCMPKT_TRACE_FORWARDING_T_L3_MPLS /* PHP on Bottom of Stack
                                                          Label. */

#define BCM_PKTIO_TRACE_FORWARDING_T_VRF    BCMPKT_TRACE_FORWARDING_T_VRF /* L3 Forwarding based on
                                                          VRF. */

#define BCM_PKTIO_TRACE_FORWARDING_T_MPLS   BCMPKT_TRACE_FORWARDING_T_MPLS /* LSR Operation (label
                                                          SWAP or PHP on a non
                                                          BOS label). */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_T_DGLP BCMPKT_TRACE_FWD_DESTINATION_T_DGLP /* Destination GLP. */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_T_NHI BCMPKT_TRACE_FWD_DESTINATION_T_NHI /* Next hop. */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_T_ECMP BCMPKT_TRACE_FWD_DESTINATION_T_ECMP /* ECMP group. */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_T_ECMP_MEMBER BCMPKT_TRACE_FWD_DESTINATION_T_ECMP_MEMBER /* ECMP member. */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_T_IPMC BCMPKT_TRACE_FWD_DESTINATION_T_IPMC /* IPMC. */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_T_L2MC BCMPKT_TRACE_FWD_DESTINATION_T_L2MC /* L2MC */

#define BCM_PKTIO_TRACE_FWD_DESTINATION_T_VLAN_FLOOD BCMPKT_TRACE_FWD_DESTINATION_T_VLAN_FLOOD /* Vlan flooding */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_PKT /* Unknown packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_CONTROL_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_CONTROL_PKT /* Ethernet Control
                                                          (8808) packets. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_OAM_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_OAM_PKT /* OAM packet that needs
                                                          to be terminated
                                                          locally. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_BFD_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_BFD_PKT /* BFD packet that needs
                                                          to be terminated
                                                          locally. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_BPDU_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_BPDU_PKT /* L2 user entry table
                                                          BPDU bit. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_PKT_IS_1588 BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_PKT_IS_1588 /* 1588 message. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2UC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2UC_PKT /* Known destination L2
                                                          unicast packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2UC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2UC_PKT /* Unknown destination L2
                                                          unicast packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2MC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2MC_PKT /* Known L2 multicast
                                                          packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2MC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L2MC_PKT /* Unknown L2 multicast
                                                          packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_L2BC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_L2BC_PKT /* L2 broadcast packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L3UC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L3UC_PKT /* Known destination L3
                                                          unicast packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L3UC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_L3UC_PKT /* Unknown destination L3
                                                          unicast packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_IPMC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_IPMC_PKT /* Known IP multicast
                                                          packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_IPMC_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_IPMC_PKT /* Unknown IP multicast
                                                          packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L2_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L2_PKT /* Point to point l2 MPLS
                                                          terminated packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MPLS_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MPLS_PKT /* MPLS packet with
                                                          unknown/invalid
                                                          forwarding label. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L3_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_L3_PKT /* MPLS forwarding label
                                                          action is PHP. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MPLS_PKT /* MPLS transit packet
                                                          with known forwarding
                                                          label. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MIM_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_MIM_PKT /* Point to point MIM
                                                          terminated packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MIM_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_MIM_PKT /* MIM tunnel error
                                                          packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_TRILL_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_TRILL_PKT /* Known egress rbridge
                                                          TRILL transit packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_TRILL_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_TRILL_PKT /* Unknown egress rbridge
                                                          TRILL packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_NIV_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_NIV_PKT /* Known destination vif
                                                          downstream NIV packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_NIV_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_UNKNOWN_NIV_PKT /* Unknown destination
                                                          vif downstream NIV
                                                          packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2GRE_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_L2GRE_PKT /* Known L2GRE packet. */

#define BCM_PKTIO_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_VXLAN_PKT BCMPKT_TRACE_PKT_RESOLUTION_VECTOR_KNOWN_VXLAN_PKT /* Known VXLAN packet. */

#define BCM_PKTIO_TRACE_DROP_REASON_BFD_TERMINATED_DROP BCMPKT_TRACE_DROP_REASON_BFD_TERMINATED_DROP /* BFD terminated drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_BPDU    BCMPKT_TRACE_DROP_REASON_BPDU /* Drop due to BPDU. */

#define BCM_PKTIO_TRACE_DROP_REASON_CFI_OR_L3DISABLE BCMPKT_TRACE_DROP_REASON_CFI_OR_L3DISABLE /* Packets dropped due to
                                                          CFI or L3 disable. */

#define BCM_PKTIO_TRACE_DROP_REASON_CML BCMPKT_TRACE_DROP_REASON_CML /* Drop due to CML. */

#define BCM_PKTIO_TRACE_DROP_REASON_COMPOSITE_ERROR BCMPKT_TRACE_DROP_REASON_COMPOSITE_ERROR /* Drop due to parity/ecc
                                                          errors. */

#define BCM_PKTIO_TRACE_DROP_REASON_CONTROL_FRAME BCMPKT_TRACE_DROP_REASON_CONTROL_FRAME /* Packet dropped due to
                                                          CONTROL_FRAME. */

#define BCM_PKTIO_TRACE_DROP_REASON_IPV4_PROTOCOL_ERROR BCMPKT_TRACE_DROP_REASON_IPV4_PROTOCOL_ERROR /* Packet dropped due to
                                                          IPV4 protocol error. */

#define BCM_PKTIO_TRACE_DROP_REASON_IPV6_PROTOCOL_ERROR BCMPKT_TRACE_DROP_REASON_IPV6_PROTOCOL_ERROR /* Packet dropped due to
                                                          IPV6 protocol error. */

#define BCM_PKTIO_TRACE_DROP_REASON_L3_DOS_ATTACK BCMPKT_TRACE_DROP_REASON_L3_DOS_ATTACK /* Packet dropped due to
                                                          L3_DOS_ATTACK. */

#define BCM_PKTIO_TRACE_DROP_REASON_L4_DOS_ATTACK BCMPKT_TRACE_DROP_REASON_L4_DOS_ATTACK /* Packet dropped due to
                                                          L4_DOS_ATTACK. */

#define BCM_PKTIO_TRACE_DROP_REASON_LAG_FAIL_LOOPBACK BCMPKT_TRACE_DROP_REASON_LAG_FAIL_LOOPBACK /* Packet dropped due to
                                                          LAG_FAIL_LOOPBACK. */

#define BCM_PKTIO_TRACE_DROP_REASON_MACSA_EQUALS_DA BCMPKT_TRACE_DROP_REASON_MACSA_EQUALS_DA /* Packet dropped due to
                                                          MACSA_EQUALS_DA. */

#define BCM_PKTIO_TRACE_DROP_REASON_IPMC_PROC BCMPKT_TRACE_DROP_REASON_IPMC_PROC /* IPMC header error
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_L2DST_DISCARD BCMPKT_TRACE_DROP_REASON_L2DST_DISCARD /* Packets dropped due to
                                                          L2 destination
                                                          discard. */

#define BCM_PKTIO_TRACE_DROP_REASON_L2SRC_DISCARD BCMPKT_TRACE_DROP_REASON_L2SRC_DISCARD /* Packets dropped due to
                                                          L2 source discard. */

#define BCM_PKTIO_TRACE_DROP_REASON_L2SRC_STATIC_MOVE BCMPKT_TRACE_DROP_REASON_L2SRC_STATIC_MOVE /* L2 source static move
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_L3DST_DISCARD BCMPKT_TRACE_DROP_REASON_L3DST_DISCARD /* L3 destination discard
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_L3SRC_DISCARD BCMPKT_TRACE_DROP_REASON_L3SRC_DISCARD /* L3 source discard
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_L3_DST_LOOKUP_MISS BCMPKT_TRACE_DROP_REASON_L3_DST_LOOKUP_MISS /* L3 destination lookup
                                                          miss drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_L3_HDR_ERROR BCMPKT_TRACE_DROP_REASON_L3_HDR_ERROR /* L3 header error drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_L3_TTL_ERROR BCMPKT_TRACE_DROP_REASON_L3_TTL_ERROR /* L3 TTL error drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_MACSA0  BCMPKT_TRACE_DROP_REASON_MACSA0 /* MACSA is zero. */

#define BCM_PKTIO_TRACE_DROP_REASON_MPLS_STAGE BCMPKT_TRACE_DROP_REASON_MPLS_STAGE /* MPLS stage drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_MULTICAST_INDEX_ERROR BCMPKT_TRACE_DROP_REASON_MULTICAST_INDEX_ERROR /* Drop due to multicast
                                                          index error. */

#define BCM_PKTIO_TRACE_DROP_REASON_MY_STATION BCMPKT_TRACE_DROP_REASON_MY_STATION /* Drop due to my station
                                                          lookup miss. */

#define BCM_PKTIO_TRACE_DROP_REASON_PFM BCMPKT_TRACE_DROP_REASON_PFM /* Port filtering mode drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_PROTCOL_PKT BCMPKT_TRACE_DROP_REASON_PROTCOL_PKT /* Drop due to protocol
                                                          packets. */

#define BCM_PKTIO_TRACE_DROP_REASON_PVLAN_VP_EFILTER BCMPKT_TRACE_DROP_REASON_PVLAN_VP_EFILTER /* Private vlan VP filter
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_ECMP_RESOLUTION_ERROR BCMPKT_TRACE_DROP_REASON_ECMP_RESOLUTION_ERROR /* Packet dropped due to
                                                          ECMP resolution error. */

#define BCM_PKTIO_TRACE_DROP_REASON_FP_CHANGE_L2_FIELDS_NO_REDIRECT_DROP BCMPKT_TRACE_DROP_REASON_FP_CHANGE_L2_FIELDS_NO_REDIRECT_DROP /* Packet dropped due to
                                                          FP change l2 fields no
                                                          redirect. */

#define BCM_PKTIO_TRACE_DROP_REASON_IFP_DROP BCMPKT_TRACE_DROP_REASON_IFP_DROP /* Packet dropped due to
                                                          IFP drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_NEXT_HOP_DROP BCMPKT_TRACE_DROP_REASON_NEXT_HOP_DROP /* Packet dropped due to
                                                          next hop indication. */

#define BCM_PKTIO_TRACE_DROP_REASON_PROTECTION_DATA_DROP BCMPKT_TRACE_DROP_REASON_PROTECTION_DATA_DROP /* Packet dropped due to
                                                          protection. */

#define BCM_PKTIO_TRACE_DROP_REASON_TUNNEL_ERROR BCMPKT_TRACE_DROP_REASON_TUNNEL_ERROR /* Packet dropped due to
                                                          tunnel errors. */

#define BCM_PKTIO_TRACE_DROP_REASON_SPANNING_TREE_STATE BCMPKT_TRACE_DROP_REASON_SPANNING_TREE_STATE /* Drop due to spanning
                                                          tree. */

#define BCM_PKTIO_TRACE_DROP_REASON_SRC_ROUTE BCMPKT_TRACE_DROP_REASON_SRC_ROUTE /* Drop due to source
                                                          route. */

#define BCM_PKTIO_TRACE_DROP_REASON_TAG_UNTAG_DISCARD BCMPKT_TRACE_DROP_REASON_TAG_UNTAG_DISCARD /* Packets dropped due to
                                                          LPORT.PORT_DIS_UNTAG
                                                          or LPORT.PORT_DIS_TAG. */

#define BCM_PKTIO_TRACE_DROP_REASON_TIME_SYNC_PKT BCMPKT_TRACE_DROP_REASON_TIME_SYNC_PKT /* Time sync packet drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TUNNEL_DECAP_ECN BCMPKT_TRACE_DROP_REASON_TUNNEL_DECAP_ECN /* Tunnel decap ECN error
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_MPLS_GAL_LABEL BCMPKT_TRACE_DROP_REASON_MPLS_GAL_LABEL /* Packet dropped due to
                                                          MPLS Generic Label
                                                          lookup miss. */

#define BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_ACTION BCMPKT_TRACE_DROP_REASON_MPLS_INVALID_ACTION /* Packet dropped due to
                                                          MPLS invalid action. */

#define BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_CW BCMPKT_TRACE_DROP_REASON_MPLS_INVALID_CW /* Packet dropped due to
                                                          MPLS invalid control
                                                          word. */

#define BCM_PKTIO_TRACE_DROP_REASON_MPLS_INVALID_PAYLOAD BCMPKT_TRACE_DROP_REASON_MPLS_INVALID_PAYLOAD /* Packet dropped due to
                                                          MPLS invalid payload. */

#define BCM_PKTIO_TRACE_DROP_REASON_MPLS_LABEL_MISS BCMPKT_TRACE_DROP_REASON_MPLS_LABEL_MISS /* Packet dropped due to
                                                          MPLS label lookup
                                                          miss. */

#define BCM_PKTIO_TRACE_DROP_REASON_MPLS_TTL_CHECK_FAIL BCMPKT_TRACE_DROP_REASON_MPLS_TTL_CHECK_FAIL /* Packet dropped due to
                                                          MPLS TTL check fail. */

#define BCM_PKTIO_TRACE_DROP_REASON_VFP BCMPKT_TRACE_DROP_REASON_VFP /* Packet dropped by VFP. */

#define BCM_PKTIO_TRACE_DROP_REASON_VLAN_CC_OR_PBT BCMPKT_TRACE_DROP_REASON_VLAN_CC_OR_PBT /* Vlan cross-connect
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_ENIFILTER BCMPKT_TRACE_DROP_REASON_ENIFILTER /* Packet dropped due to
                                                          ingress port not in
                                                          VLAN membership. */

#define BCM_PKTIO_TRACE_DROP_REASON_INVALID_TPID BCMPKT_TRACE_DROP_REASON_INVALID_TPID /* Packet dropped due to
                                                          TPID mismatch. */

#define BCM_PKTIO_TRACE_DROP_REASON_INVALID_VLAN BCMPKT_TRACE_DROP_REASON_INVALID_VLAN /* Packet dropped due to
                                                          VLAN not valid. */

#define BCM_PKTIO_TRACE_DROP_REASON_PVLAN_VID_MISMATCH BCMPKT_TRACE_DROP_REASON_PVLAN_VID_MISMATCH /* Packet dropped due to
                                                          PVLAN VID mismatch. */

#define BCM_PKTIO_TRACE_DROP_REASON_VXLT_MISS BCMPKT_TRACE_DROP_REASON_VXLT_MISS /* VXLT miss drop
                                                          condition. */

#define BCM_PKTIO_TRACE_DROP_REASON_HIGIG_MH_TYPE1 BCMPKT_TRACE_DROP_REASON_HIGIG_MH_TYPE1 /* HiGig module header
                                                          type 1 drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_DISC_STAGE BCMPKT_TRACE_DROP_REASON_DISC_STAGE /* HiGig discard drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_SW1_INVALID_VLAN BCMPKT_TRACE_DROP_REASON_SW1_INVALID_VLAN /* HiGig invalid VLAN
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_HIGIG_HDR_ERROR BCMPKT_TRACE_DROP_REASON_HIGIG_HDR_ERROR /* HiGig Header error
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_LAG_FAILOVER BCMPKT_TRACE_DROP_REASON_LAG_FAILOVER /* This is looped back
                                                          packet. */

#define BCM_PKTIO_TRACE_DROP_REASON_CLASS_BASED_SM BCMPKT_TRACE_DROP_REASON_CLASS_BASED_SM /* Class based learning
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_BAD_UDP_CHECKSUM BCMPKT_TRACE_DROP_REASON_BAD_UDP_CHECKSUM /* Bad udp checksum drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_NIV_FORWARDING BCMPKT_TRACE_DROP_REASON_NIV_FORWARDING /* NIV Forwarding drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_NIV_RPF_CHECK_FAIL BCMPKT_TRACE_DROP_REASON_NIV_RPF_CHECK_FAIL /* NIV RPF check fail
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_HEADER_VERSION_NONZERO BCMPKT_TRACE_DROP_REASON_TRILL_HEADER_VERSION_NONZERO /* Trill Header Version
                                                          Nonzero drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_ADJACENCY_CHECK_FAIL BCMPKT_TRACE_DROP_REASON_TRILL_ADJACENCY_CHECK_FAIL /* Trill Adjacency Check
                                                          Fail drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_RBRIDGE_LOOKUP_MISS BCMPKT_TRACE_DROP_REASON_TRILL_RBRIDGE_LOOKUP_MISS /* Trill RBridge Lookup
                                                          Miss drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_UC_HDR_MC_MACDA BCMPKT_TRACE_DROP_REASON_TRILL_UC_HDR_MC_MACDA /* Trill UC HDR MC MACDA
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_SLOWPATH BCMPKT_TRACE_DROP_REASON_TRILL_SLOWPATH /* Trill Slowpath drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_CORE_IS_IS_PKT BCMPKT_TRACE_DROP_REASON_CORE_IS_IS_PKT /* Core IS-IS Packet
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_RPF_CHECK_FAIL BCMPKT_TRACE_DROP_REASON_TRILL_RPF_CHECK_FAIL /* Trill RPF Check Fail
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_PKT_WITH_SNAP_ENCAP BCMPKT_TRACE_DROP_REASON_TRILL_PKT_WITH_SNAP_ENCAP /* Trill Packet with SNAP
                                                          Encap drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_ING_RBRIDGE_EQ_EGR_RBRIDGE BCMPKT_TRACE_DROP_REASON_TRILL_ING_RBRIDGE_EQ_EGR_RBRIDGE /* Trill Packet with
                                                          Ingress RBridge equal
                                                          to Egress RBridge
                                                          drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_TRILL_HOPCOUNT_CHECK_FAIL BCMPKT_TRACE_DROP_REASON_TRILL_HOPCOUNT_CHECK_FAIL /* Trill Hop Count check
                                                          fail drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_INT_DATAPLANE_PROBE BCMPKT_TRACE_DROP_REASON_INT_DATAPLANE_PROBE /* INT dataplane probe */

#define BCM_PKTIO_TRACE_DROP_REASON_INT_ERROR BCMPKT_TRACE_DROP_REASON_INT_ERROR /* INT packet error */

#define BCM_PKTIO_TRACE_DROP_REASON_INVALID_GSH_DROP BCMPKT_TRACE_DROP_REASON_INVALID_GSH_DROP /* Invalid GSH packet
                                                          dropped */

#define BCM_PKTIO_TRACE_DROP_REASON_INVALID_NON_GSH_DROP BCMPKT_TRACE_DROP_REASON_INVALID_NON_GSH_DROP /* Invalid non-GSH packet
                                                          dropped */

#define BCM_PKTIO_TRACE_DROP_REASON_SER_DROP BCMPKT_TRACE_DROP_REASON_SER_DROP /* Drop due to parity/ecc
                                                          errors */

#define BCM_PKTIO_TRACE_DROP_REASON_SRV6_PROC_DROP BCMPKT_TRACE_DROP_REASON_SRV6_PROC_DROP /* Packet dropped due to
                                                          SRV6 process error. */

#define BCM_PKTIO_TRACE_DROP_REASON_SRV6_VALIDATION_DROP BCMPKT_TRACE_DROP_REASON_SRV6_VALIDATION_DROP /* Packet dropped due to
                                                          SRV6 validation error. */

#define BCM_PKTIO_TRACE_DROP_REASON_VXLAN_TUNNEL_ERROR_DROP BCMPKT_TRACE_DROP_REASON_VXLAN_TUNNEL_ERROR_DROP /* Packet dropped due to
                                                          VXLAN Tunnel Error. */

#define BCM_PKTIO_TRACE_DROP_REASON_VXLAN_VN_ID_LOOKUP_MISS BCMPKT_TRACE_DROP_REASON_VXLAN_VN_ID_LOOKUP_MISS /* Packet dropped due
                                                          VXLAN VN_ID lookup
                                                          miss. */

#define BCM_PKTIO_TRACE_DROP_REASON_UNKNOWN_RH_WITH_NONZERO_SL_DROP BCMPKT_TRACE_DROP_REASON_UNKNOWN_RH_WITH_NONZERO_SL_DROP /* Packet dropped due to
                                                          unknown routing type
                                                          in SRV6 packet with
                                                          nonzeroSegmentLeft
                                                          field. */

#define BCM_PKTIO_TRACE_DROP_REASON_COUNT   BCMPKT_TRACE_DROP_REASON_COUNT /* TRACE DROP REASON TYPE
                                                          NUMBER */

#define BCM_PKTIO_TRACE_COUNTER_RIPD4   BCMPKT_TRACE_COUNTER_RIPD4 /* L3 pkt discarded due to
                                                      resource. */

#define BCM_PKTIO_TRACE_COUNTER_RIPC4   BCMPKT_TRACE_COUNTER_RIPC4 /* Forwarded IPv4 packets. */

#define BCM_PKTIO_TRACE_COUNTER_RIPHE4  BCMPKT_TRACE_COUNTER_RIPHE4 /* IP Header Error packets. */

#define BCM_PKTIO_TRACE_COUNTER_IMRP4   BCMPKT_TRACE_COUNTER_IMRP4 /* Routed IPv4 Muliticast
                                                      packets. */

#define BCM_PKTIO_TRACE_COUNTER_RIPD6   BCMPKT_TRACE_COUNTER_RIPD6 /* L3 pkt discarded due to
                                                      resource. */

#define BCM_PKTIO_TRACE_COUNTER_RIPC6   BCMPKT_TRACE_COUNTER_RIPC6 /* Forwarded IPv6 packets. */

#define BCM_PKTIO_TRACE_COUNTER_RIPHE6  BCMPKT_TRACE_COUNTER_RIPHE6 /* IP Header Error packets. */

#define BCM_PKTIO_TRACE_COUNTER_IMRP6   BCMPKT_TRACE_COUNTER_IMRP6 /* Routed IPv6 Muliticast
                                                      packets. */

#define BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_ACH_ERROR BCMPKT_TRACE_COUNTER_BFD_UNKNOWN_ACH_ERROR /* BFD Version number of
                                                          ACH header is not zero
                                                          or ACH channel type is
                                                          unknown. */

#define BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_CONTROL_PACKET BCMPKT_TRACE_COUNTER_BFD_UNKNOWN_CONTROL_PACKET /* Unrecognized control
                                                          packet received from
                                                          control channel, which
                                                          can be PW VCCVtype
                                                          1/2/3, MPLS-TP control
                                                          channel, etc. */

#define BCM_PKTIO_TRACE_COUNTER_BFD_UNKNOWN_VER_OR_DISCR BCMPKT_TRACE_COUNTER_BFD_UNKNOWN_VER_OR_DISCR /* BFD Unknown version or
                                                          discard dropped. */

#define BCM_PKTIO_TRACE_COUNTER_BLOCK_MASK_DROP BCMPKT_TRACE_COUNTER_BLOCK_MASK_DROP /* Drop due to block
                                                          masks. */

#define BCM_PKTIO_TRACE_COUNTER_DSFRAG  BCMPKT_TRACE_COUNTER_DSFRAG /* DOS fragment error
                                                      packets. */

#define BCM_PKTIO_TRACE_COUNTER_DSICMP  BCMPKT_TRACE_COUNTER_DSICMP /* DOS ICMP error packets. */

#define BCM_PKTIO_TRACE_COUNTER_DSL2HE  BCMPKT_TRACE_COUNTER_DSL2HE /* Dos Attack L2 Packets. */

#define BCM_PKTIO_TRACE_COUNTER_IMBP    BCMPKT_TRACE_COUNTER_IMBP /* Bridged Multicast pkts. */

#define BCM_PKTIO_TRACE_COUNTER_LAGLUP  BCMPKT_TRACE_COUNTER_LAGLUP /* LAG failover loopback pkt. */

#define BCM_PKTIO_TRACE_COUNTER_LAGLUPD BCMPKT_TRACE_COUNTER_LAGLUPD /* LAG failover loopback pkt
                                                      discarded. */

#define BCM_PKTIO_TRACE_COUNTER_MTU_CHECK_FAIL BCMPKT_TRACE_COUNTER_MTU_CHECK_FAIL /* Pkts dropped due to
                                                          MTU check fail. */

#define BCM_PKTIO_TRACE_COUNTER_PARITYD BCMPKT_TRACE_COUNTER_PARITYD /* Pkts dropped due to parity
                                                      error. */

#define BCM_PKTIO_TRACE_COUNTER_PDISC   BCMPKT_TRACE_COUNTER_PDISC /* Non-IP packet discard
                                                      dropped. */

#define BCM_PKTIO_TRACE_COUNTER_RDROP   BCMPKT_TRACE_COUNTER_RDROP /* Portbitmap zero drop
                                                      condition. */

#define BCM_PKTIO_TRACE_COUNTER_RIMDR   BCMPKT_TRACE_COUNTER_RIMDR /* Multicast (L2+L3) packets
                                                      that are dropped. */

#define BCM_PKTIO_TRACE_COUNTER_RPORTD  BCMPKT_TRACE_COUNTER_RPORTD /* Pkts dropped as ingress
                                                      port's SP state is not
                                                      forwarding. */

#define BCM_PKTIO_TRACE_COUNTER_RTUN    BCMPKT_TRACE_COUNTER_RTUN /* Good Tunnel terminated
                                                      packets. */

#define BCM_PKTIO_TRACE_COUNTER_RTUNE   BCMPKT_TRACE_COUNTER_RTUNE /* Receive tunnel error
                                                      packets. */

#define BCM_PKTIO_TRACE_COUNTER_RUC BCMPKT_TRACE_COUNTER_RUC /* Good Unicast Packets. */

#define BCM_PKTIO_TRACE_COUNTER_SRC_PORT_KNOCKOUT_DROP BCMPKT_TRACE_COUNTER_SRC_PORT_KNOCKOUT_DROP /* Drop due to source
                                                          port knockout. */

#define BCM_PKTIO_TRACE_COUNTER_RDISC   BCMPKT_TRACE_COUNTER_RDISC /* IBP discard and CBP full. */

#define BCM_PKTIO_TRACE_COUNTER_RFILDR  BCMPKT_TRACE_COUNTER_RFILDR /* Packets dropped by FP. */

#define BCM_PKTIO_TRACE_COUNTER_IRPSE   BCMPKT_TRACE_COUNTER_IRPSE /* HiGig IPIC pause receive
                                                      counter. */

#define BCM_PKTIO_TRACE_COUNTER_IRHOL   BCMPKT_TRACE_COUNTER_IRHOL /* HiGig End-to-End HOL
                                                      receive packet counter. */

#define BCM_PKTIO_TRACE_COUNTER_IRIBP   BCMPKT_TRACE_COUNTER_IRIBP /* HiGig End-to-End IBP
                                                      receive packet counter. */

#define BCM_PKTIO_TRACE_COUNTER_DSL3HE  BCMPKT_TRACE_COUNTER_DSL3HE /* DOS L3 header error
                                                      packets (only applicable
                                                      to packet from 10GE port). */

#define BCM_PKTIO_TRACE_COUNTER_IUNKHDR BCMPKT_TRACE_COUNTER_IUNKHDR /* Unknown HiGig header type
                                                      packet (only applicable to
                                                      packet from HiGig port). */

#define BCM_PKTIO_TRACE_COUNTER_DSL4HE  BCMPKT_TRACE_COUNTER_DSL4HE /* DOS L4 header error
                                                      packets (only applicable
                                                      to packet from 10GE port). */

#define BCM_PKTIO_TRACE_COUNTER_IMIRROR BCMPKT_TRACE_COUNTER_IMIRROR /* HiGig mirror packet (only
                                                      applicable to packet from
                                                      HiGig port). */

#define BCM_PKTIO_TRACE_COUNTER_MTUERR  BCMPKT_TRACE_COUNTER_MTUERR /* Packets trapped to CPU due
                                                      to egress L3 MTU
                                                      violation. */

#define BCM_PKTIO_TRACE_COUNTER_VLANDR  BCMPKT_TRACE_COUNTER_VLANDR /* Receive VLAN drop cases. */

#define BCM_PKTIO_TRACE_COUNTER_HGHDRE  BCMPKT_TRACE_COUNTER_HGHDRE /* HiGig Header error
                                                      packets. */

#define BCM_PKTIO_TRACE_COUNTER_MCIDXE  BCMPKT_TRACE_COUNTER_MCIDXE /* Multicast Index error
                                                      packets. */

#define BCM_PKTIO_TRACE_COUNTER_RHGUC   BCMPKT_TRACE_COUNTER_RHGUC /* Receive HiGig Unicast
                                                      packet. */

#define BCM_PKTIO_TRACE_COUNTER_RHGMC   BCMPKT_TRACE_COUNTER_RHGMC /* Receive HiGig non-Unicast
                                                      packet. */

#define BCM_PKTIO_TRACE_COUNTER_URPF    BCMPKT_TRACE_COUNTER_URPF /* Unicast Reverse Path
                                                      Forwarding. */

#define BCM_PKTIO_TRACE_COUNTER_VFPDR   BCMPKT_TRACE_COUNTER_VFPDR /* VLAN FP drop case. */

#define BCM_PKTIO_TRACE_COUNTER_DSTDISC BCMPKT_TRACE_COUNTER_DSTDISC /* L2/L3 lookup DST_DISCARD
                                                      drop. */

#define BCM_PKTIO_TRACE_COUNTER_CBLDROP BCMPKT_TRACE_COUNTER_CBLDROP /* Class based learning drop. */

#define BCM_PKTIO_TRACE_COUNTER_MACLMT_NODROP BCMPKT_TRACE_COUNTER_MACLMT_NODROP /* MAC limit exceeded and
                                                          packet not dropped. */

#define BCM_PKTIO_TRACE_COUNTER_MACLMT_DROP BCMPKT_TRACE_COUNTER_MACLMT_DROP /* MAC limit exceeded and
                                                          packet dropped. */

#define BCM_PKTIO_TRACE_COUNTER_VNTAG_ERROR BCMPKT_TRACE_COUNTER_VNTAG_ERROR /* VNTAG Error packet
                                                          dropped. */

#define BCM_PKTIO_TRACE_COUNTER_NIV_FORWARDING_DROP BCMPKT_TRACE_COUNTER_NIV_FORWARDING_DROP /* NIV Forwarding Error
                                                          dropped. */

#define BCM_PKTIO_TRACE_COUNTER_OFFSET_NONTRILL_FRAME_ON_NW_PORT_DROP BCMPKT_TRACE_COUNTER_OFFSET_NONTRILL_FRAME_ON_NW_PORT_DROP /* Non-Trill Frame on
                                                          Network Port dropped. */

#define BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_FRAME_ON_ACCESS_PORT_DROP BCMPKT_TRACE_COUNTER_OFFSET_TRILL_FRAME_ON_ACCESS_PORT_DROP /* Trill Frame on Access
                                                          Port dropped. */

#define BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_ERRORS_DROP BCMPKT_TRACE_COUNTER_OFFSET_TRILL_ERRORS_DROP /* Trill errors dropped. */

#define BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_RBRIDGE_LOOKUP_MISS_DROP BCMPKT_TRACE_COUNTER_OFFSET_TRILL_RBRIDGE_LOOKUP_MISS_DROP /* Trill RBridge Lookup
                                                          Miss dropped. */

#define BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_HOPCOUNT_CHECK_FAIL BCMPKT_TRACE_COUNTER_OFFSET_TRILL_HOPCOUNT_CHECK_FAIL /* Trill HopCount check
                                                          fail dropped. */

#define BCM_PKTIO_TRACE_COUNTER_OFFSET_TRILL_RPF_CHECK_FAIL_DROP BCMPKT_TRACE_COUNTER_OFFSET_TRILL_RPF_CHECK_FAIL_DROP /* Trill RPF Check Fail
                                                          dropped. */

#define BCM_PKTIO_TRACE_COUNTER_IRVOQFC BCMPKT_TRACE_COUNTER_IRVOQFC /* Virtual-Output-Queue-based
                                                      Flow-Control-Message
                                                      receive packet counter. */

#define BCM_PKTIO_TRACE_COUNTER_ECMP_NOT_RESOLVED BCMPKT_TRACE_COUNTER_ECMP_NOT_RESOLVED /* ECMP not resolved. */

#define BCM_PKTIO_TRACE_COUNTER_COUNT   BCMPKT_TRACE_COUNTER_COUNT /* TRACE COUNTER TYPE NUMBER */

#define BCM_PKTIO_TRACE_DOP_FID_INVALID BCMPKT_TRACE_DOP_FID_INVALID /* Invalid
                                                      BCM_PKTIO_TRACE_DOP FID
                                                      indicator */

#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR /* IPARSER0_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH /* IPARSER0_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT /* IPARSER0_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_PKT_DATA BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_PKT_DATA /* IPARSER0_HME_STAGE0_DOP_STAGE0_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR /* IPARSER1_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH /* IPARSER1_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT /* IPARSER1_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_PKT_DATA /* IPARSER1_HME_STAGE0_DOP_STAGE0_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR /* IPARSER1_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH /* IPARSER1_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT /* IPARSER1_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_PKT_DATA /* IPARSER1_HME_STAGE1_DOP_STAGE1_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR /* IPARSER1_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH /* IPARSER1_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT /* IPARSER1_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_PKT_DATA /* IPARSER1_HME_STAGE2_DOP_STAGE2_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR /* IPARSER1_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH /* IPARSER1_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT /* IPARSER1_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_PKT_DATA /* IPARSER1_HME_STAGE3_DOP_STAGE3_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR /* IPARSER1_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH /* IPARSER1_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT /* IPARSER1_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_PKT_DATA /* IPARSER1_HME_STAGE4_DOP_STAGE4_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_HFE_PROFILE_PTR /* IPARSER1_HME_STAGE5_DOP_STAGE5_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_TCAM_TCAM_MATCH /* IPARSER1_HME_STAGE5_DOP_STAGE5_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_SHIFT_AMOUNT /* IPARSER1_HME_STAGE5_DOP_STAGE5_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_PKT_DATA /* IPARSER1_HME_STAGE5_DOP_STAGE5_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR /* IPARSER2_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH /* IPARSER2_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT /* IPARSER2_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_PKT_DATA /* IPARSER2_HME_STAGE0_DOP_STAGE0_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR /* IPARSER2_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH /* IPARSER2_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT /* IPARSER2_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_PKT_DATA /* IPARSER2_HME_STAGE1_DOP_STAGE1_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR /* IPARSER2_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH /* IPARSER2_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT /* IPARSER2_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_PKT_DATA /* IPARSER2_HME_STAGE2_DOP_STAGE2_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR /* IPARSER2_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH /* IPARSER2_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT /* IPARSER2_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_PKT_DATA /* IPARSER2_HME_STAGE3_DOP_STAGE3_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR /* IPARSER2_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH /* IPARSER2_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT /* IPARSER2_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT. */

#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_PKT_DATA /* IPARSER2_HME_STAGE4_DOP_STAGE4_PKT_DATA. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_KEY /* MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_KEY /* MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_PKT_RD /* MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_KEY /* MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_KEY /* MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_KEY /* MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_PKT_RD /* MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_KEY /* MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_PKT_RD /* MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_KEY /* MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_KEY /* MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_PKT_RD /* MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP_TCAM_ONLY_KEY BCMPKT_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP_TCAM_ONLY_KEY /* MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP_TCAM_ONLY_KEY. */

#define BCM_PKTIO_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_MATCH_INDEX BCMPKT_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_MATCH_INDEX /* MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_TCAM_MATCH BCMPKT_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_TCAM_MATCH /* MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_KEY /* MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_KEY /* MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_KEY /* MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_KEY /* MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP_NET_LAYER_TCAM_0_KEY BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP_NET_LAYER_TCAM_0_KEY /* FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP_NET_LAYER_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_MATCH_INDEX /* FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_MATCH_INDEX /* FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_MATCH_INDEX /* FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_TCAM_MATCH /* FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_TCAM_MATCH /* FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_TCAM_MATCH /* FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_NORM_FD_NORM_DOP_OUT_FD_NORM_FLD_BUS BCMPKT_TRACE_DOP_FLEX_DIGEST_NORM_FD_NORM_DOP_OUT_FD_NORM_FLD_BUS /* FLEX_DIGEST_NORM_FD_NORM_DOP_OUT_FD_NORM_FLD_BUS. */

#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_HASH_FD_HASH_DOP_OUT_ING_HASH_BUS BCMPKT_TRACE_DOP_FLEX_DIGEST_HASH_FD_HASH_DOP_OUT_ING_HASH_BUS /* FLEX_DIGEST_HASH_FD_HASH_DOP_OUT_ING_HASH_BUS. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* FLEX_QOS_PHB_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX /* FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH /* FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_KEY /* MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_KEY /* MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_KEY /* MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_KEY /* MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_KEY /* MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_KEY /* MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_KEY /* MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_KEY /* MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_KEY /* MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_KEY /* MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_KEY /* MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_KEY /* MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_KEY /* MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_KEY /* MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_PKT_RD /* MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_KEY /* MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_KEY /* MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_PKT_RD /* MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX /* MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH /* MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B0 /* FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B1 /* FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B2 /* FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B3 /* FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A0 /* FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A1 /* FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A2 /* FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A3 /* FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A3. */

#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_KEY /* MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_PKT_RD /* MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_KEY /* MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_KEY /* MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_PKT_RD /* MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_KEY /* MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_PKT_RD /* MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_KEY /* MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_PKT_RD /* MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_KEY /* MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_PKT_RD /* MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_KEY /* MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_PKT_RD /* MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_KEY /* MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_PKT_RD /* MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_KEY /* MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_PKT_RD /* MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_KEY /* MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_PKT_RD /* MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_KEY /* MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_PKT_RD /* MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_KEY /* MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_PKT_RD /* MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_KEY /* MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_PKT_RD /* MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_KEY /* MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_PKT_RD /* MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_KEY /* MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_PKT_RD /* MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_KEY /* MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_PKT_RD /* MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_KEY /* MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_PKT_RD /* MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_KEY /* MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_PKT_RD /* MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_KEY /* MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_PKT_RD /* MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_KEY /* MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_PKT_RD /* MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_KEY /* MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_PKT_RD /* MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_KEY /* MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_PKT_RD /* MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_KEY /* MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_PKT_RD /* MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_KEY /* MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_PKT_RD /* MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_KEY /* MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_PKT_RD /* MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_KEY /* MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_PKT_RD /* MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_KEY /* MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_PKT_RD /* MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_KEY /* MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_PKT_RD /* MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_KEY /* MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_PKT_RD /* MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_KEY /* MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_PKT_RD /* MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_KEY /* MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_PKT_RD /* MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_KEY /* MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_PKT_RD /* MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_KEY /* MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_PKT_RD /* MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_KEY /* MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_PKT_RD /* MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_KEY /* MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_KEY /* MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_PKT_RD /* MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_KEY /* MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_KEY /* MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_PKT_RD /* MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_KEY /* MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_PKT_RD /* MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_KEY /* MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_PKT_RD /* MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_KEY /* MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_PKT_RD /* MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_KEY /* MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_PKT_RD /* MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_KEY /* MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_PKT_RD /* MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_KEY /* MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_KEY /* MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_PKT_RD /* MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_KEY /* MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_PKT_RD /* MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_KEY /* MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_PKT_RD /* MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_KEY /* MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_PKT_RD /* MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_MATCH_INDEX /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_TCAM_MATCH /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_MATCH_INDEX /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_TCAM_MATCH /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_MATCH_INDEX /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_TCAM_MATCH /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_MATCH_INDEX /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_TCAM_MATCH /* MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_MATCH_INDEX /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_TCAM_MATCH /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_MATCH_INDEX /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_TCAM_MATCH /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_MATCH_INDEX /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_TCAM_MATCH /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_MATCH_INDEX /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_TCAM_MATCH /* MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_MATCH_INDEX /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_TCAM_MATCH /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_MATCH_INDEX /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_TCAM_MATCH /* MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_MATCH_INDEX /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_TCAM_MATCH /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_MATCH_INDEX /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_TCAM_MATCH /* MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_MATCH_INDEX /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_TCAM_MATCH /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_MATCH_INDEX /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_TCAM_MATCH /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_MATCH_INDEX /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_TCAM_MATCH /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_MATCH_INDEX /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_TCAM_MATCH /* MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_MATCH_INDEX /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_TCAM_MATCH /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_MATCH_INDEX /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_TCAM_MATCH /* MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_MATCH_INDEX /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_TCAM_MATCH /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_MATCH_INDEX /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_TCAM_MATCH /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_MATCH_INDEX /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_TCAM_MATCH /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_MATCH_INDEX /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_TCAM_MATCH /* MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_MATCH_INDEX /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_TCAM_MATCH /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_MATCH_INDEX /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_TCAM_MATCH /* MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH /* MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_MATCH_INDEX /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_TCAM_MATCH /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_MATCH_INDEX /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_TCAM_MATCH /* MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_MATCH_INDEX /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_TCAM_MATCH /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_MATCH_INDEX /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_TCAM_MATCH /* MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX /* ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA /* ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX /* ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_HASH BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_HASH /* ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_HASH. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_MODULO_OFFSET BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_MODULO_OFFSET /* ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_MODULO_OFFSET. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B1 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B1 /* ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B1. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B0 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B0 /* ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B0. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A1 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A1 /* ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A1. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A0 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A0 /* ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A0. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_ETR_HIT BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_ETR_HIT /* ETRAP_ETR_HIT_RTAG_HASH_DOP_ETR_HIT. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_FLW_OFMT_FLOW_TABLE_HIT BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_FLW_OFMT_FLOW_TABLE_HIT /* ETRAP_ETR_OUT_DOP_FLW_OFMT_FLOW_TABLE_HIT. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_EOP_FLOW_TABLE_HIT_PM2 BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_EOP_FLOW_TABLE_HIT_PM2 /* ETRAP_ETR_OUT_DOP_EOP_FLOW_TABLE_HIT_PM2. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_CNG BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_CNG /* ETRAP_ETR_OUT_DOP_ETR_CNG. */

#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_VALID BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_VALID /* ETRAP_ETR_OUT_DOP_ETR_VALID. */

#define BCM_PKTIO_TRACE_DOP_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP_CURRENT_TIME BCMPKT_TRACE_DOP_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP_CURRENT_TIME /* DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP_CURRENT_TIME. */

#define BCM_PKTIO_TRACE_DOP_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP_FLOWSET_INDEX BCMPKT_TRACE_DOP_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP_FLOWSET_INDEX /* DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP_FLOWSET_INDEX. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX /* ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA /* ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX /* ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_HASH BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_HASH /* ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_HASH. */

#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_MODULO_OFFSET BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_MODULO_OFFSET /* ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_MODULO_OFFSET. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B0 /* FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B1 /* FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B2 /* FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B3 /* FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A0 /* FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A1 /* FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A2 /* FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A3 /* FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A3. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_KEY /* MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_KEY /* MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_KEY /* MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_KEY /* MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP_CURRENT_TIME BCMPKT_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP_CURRENT_TIME /* IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP_CURRENT_TIME. */

#define BCM_PKTIO_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP_FLOWSET_INDEX BCMPKT_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP_FLOWSET_INDEX /* IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP_FLOWSET_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP_MIRROR_BUS BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP_MIRROR_BUS /* IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP_MIRROR_BUS. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_0_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_0_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_0_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_1_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_1_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_1_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_2_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_2_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_2_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_3_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_3_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_3_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_4_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_4_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_4_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_5_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_5_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_5_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_6_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_6_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_6_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_7_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_7_RDATA /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_7_RDATA. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_0_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_0_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_0_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_1_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_1_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_1_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_2_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_2_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_2_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_3_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_3_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_3_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_4_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_4_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_4_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_5_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_5_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_5_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_6_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_6_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_6_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_7_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_7_WINNER /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_7_WINNER. */

#define BCM_PKTIO_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX /* IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH /* IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP_CPU_COS_MAP_TCAM_KEY BCMPKT_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP_CPU_COS_MAP_TCAM_KEY /* IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP_CPU_COS_MAP_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_MATCH_INDEX /* IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_TCAM_MATCH /* IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY /* IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_MATCH_INDEX /* IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_TCAM_MATCH /* IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP_OUT_MPB_FLEX_BUS BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP_OUT_MPB_FLEX_BUS /* IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP_OUT_MPB_FLEX_BUS. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP_OUT_CCBI_B_BUS BCMPKT_TRACE_DOP_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP_OUT_CCBI_B_BUS /* IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP_OUT_CCBI_B_BUS. */

#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP_OUT_MPB_FIXED_BUS BCMPKT_TRACE_DOP_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP_OUT_MPB_FIXED_BUS /* IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP_OUT_MPB_FIXED_BUS. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP_ING_FLEX_CTR_BUS BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP_ING_FLEX_CTR_BUS /* FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP_ING_FLEX_CTR_BUS. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP_COUNTER_EOP_BUFFER_11_PKT_WDATA_COUNTER_EOP_BUFFER_11_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP_COUNTER_EOP_BUFFER_11_PKT_WDATA_COUNTER_EOP_BUFFER_11_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP_COUNTER_EOP_BUFFER_11_PKT_WDATA_COUNTER_EOP_BUFFER_11_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP_COUNTER_EOP_BUFFER_8_PKT_WDATA_COUNTER_EOP_BUFFER_8_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP_COUNTER_EOP_BUFFER_8_PKT_WDATA_COUNTER_EOP_BUFFER_8_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP_COUNTER_EOP_BUFFER_8_PKT_WDATA_COUNTER_EOP_BUFFER_8_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP_COUNTER_EOP_BUFFER_9_PKT_WDATA_COUNTER_EOP_BUFFER_9_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP_COUNTER_EOP_BUFFER_9_PKT_WDATA_COUNTER_EOP_BUFFER_9_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP_COUNTER_EOP_BUFFER_9_PKT_WDATA_COUNTER_EOP_BUFFER_9_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP_COUNTER_EOP_BUFFER_10_PKT_WDATA_COUNTER_EOP_BUFFER_10_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP_COUNTER_EOP_BUFFER_10_PKT_WDATA_COUNTER_EOP_BUFFER_10_PKT_WR /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP_COUNTER_EOP_BUFFER_10_PKT_WDATA_COUNTER_EOP_BUFFER_10_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP_FIELD_EXTRACTION_PROFILE_CONTROL_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP_FIELD_EXTRACTION_PROFILE_CONTROL_PKT_ADDR /* EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP_FIELD_EXTRACTION_PROFILE_CONTROL_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER0_BUS BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER0_BUS /* EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER0_BUS. */

#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER1_BUS BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER1_BUS /* EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER1_BUS. */

#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP_EGR_INT_CN_UPDATE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP_EGR_INT_CN_UPDATE_PKT_ADDR /* EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP_EGR_INT_CN_UPDATE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP_FORWARDING_TYPE_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP_FORWARDING_TYPE_TABLE_PKT_ADDR /* EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP_FORWARDING_TYPE_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_MPB_FIXED_DOP_MPB_FIXED BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_MPB_FIXED_DOP_MPB_FIXED /* EPRE_EDEV_CONFIG_MPB_FIXED_DOP_MPB_FIXED. */

#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_CCBE_DOP_CCBE_BUS BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_CCBE_DOP_CCBE_BUS /* EPRE_EDEV_CONFIG_CCBE_DOP_CCBE_BUS. */

#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP_EGR_TABLE_INDEX_UPDATE_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP_EGR_TABLE_INDEX_UPDATE_PROFILE_PKT_ADDR /* EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP_EGR_TABLE_INDEX_UPDATE_PROFILE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP_MIRROR_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP_MIRROR_ATTRIBUTES_TABLE_PKT_ADDR /* EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP_MIRROR_ATTRIBUTES_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_PDD_PROFILE_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_PDD_PROFILE_TABLE_PKT_ADDR /* EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_PDD_PROFILE_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_MPB_FLEX BCMPKT_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_MPB_FLEX /* EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_MPB_FLEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_ADDR /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_RD /* MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_TS_PROFILE_INDEX BCMPKT_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_TS_PROFILE_INDEX /* EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_TS_PROFILE_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_INGRESS_TIMESTAMP_BUS BCMPKT_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_INGRESS_TIMESTAMP_BUS /* EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_INGRESS_TIMESTAMP_BUS. */

#define BCM_PKTIO_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_TIMESTAMP_COUNT BCMPKT_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_TIMESTAMP_COUNT /* EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_TIMESTAMP_COUNT. */

#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBER_INDEX BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBER_INDEX /* EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBER_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_FIELD_BITMAP_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_FIELD_BITMAP_PKT_ADDR /* EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_FIELD_BITMAP_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_UNTAG_BITMAP_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_UNTAG_BITMAP_PKT_ADDR /* EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_UNTAG_BITMAP_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_STATE_PROFILE_LOWER_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_STATE_PROFILE_LOWER_PKT_ADDR /* EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_STATE_PROFILE_LOWER_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBERSHIP_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBERSHIP_PROFILE_PKT_ADDR /* EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBERSHIP_PROFILE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B0 /* FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B1 /* FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B2 /* FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B3 /* FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A0 /* FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A1 /* FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A2 /* FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A3 /* FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A3. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_KEY /* MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_KEY /* MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_KEY /* MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_KEY /* MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_KEY. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */

#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */

#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP_OUT_EGR_SCR_MIRROR2EDIT_CTRL_BUS BCMPKT_TRACE_DOP_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP_OUT_EGR_SCR_MIRROR2EDIT_CTRL_BUS /* EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP_OUT_EGR_SCR_MIRROR2EDIT_CTRL_BUS. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP_LTS_TCAM_ONLY_KEY BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP_LTS_TCAM_ONLY_KEY /* QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP_LTS_TCAM_ONLY_KEY. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_MATCH_INDEX BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_MATCH_INDEX /* QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH /* QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_4_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_4_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_4_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_3_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_3_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_2_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_2_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_1_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_1_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_0_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_0_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_1_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_1_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_0_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_0_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_4_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_4_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_4_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_3_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_3_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_3_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_2_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_2_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_2_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_1_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_1_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_1_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_0_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_0_PKT_ADDR /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_0_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP_TCAM_4_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP_TCAM_4_A_KEY /* EDIT_CTRL_ZONE_4_TCAM_KEY_DOP_TCAM_4_A_KEY. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP_TCAM_3_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP_TCAM_3_A_KEY /* EDIT_CTRL_ZONE_3_TCAM_KEY_DOP_TCAM_3_A_KEY. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP_TCAM_1_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP_TCAM_1_A_KEY /* EDIT_CTRL_ZONE_1_TCAM_KEY_DOP_TCAM_1_A_KEY. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP_TCAM_2_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP_TCAM_2_A_KEY /* EDIT_CTRL_ZONE_2_TCAM_KEY_DOP_TCAM_2_A_KEY. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP_TCAM_0_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP_TCAM_0_A_KEY /* EDIT_CTRL_ZONE_0_TCAM_KEY_DOP_TCAM_0_A_KEY. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_TCAM_MATCH /* EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_MATCH_INDEX /* EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_TCAM_MATCH /* EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_MATCH_INDEX /* EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_TCAM_MATCH /* EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_MATCH_INDEX /* EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_TCAM_MATCH /* EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_MATCH_INDEX /* EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_TCAM_MATCH /* EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_MATCH_INDEX /* EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_TCAM_MATCH /* EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_MATCH_INDEX /* EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_TCAM_MATCH /* EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_MATCH_INDEX /* EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_TCAM_MATCH /* EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_MATCH_INDEX /* EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_TCAM_MATCH /* EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_MATCH_INDEX /* EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_TCAM_MATCH /* EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_MATCH_INDEX /* EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_TCAM_MATCH /* EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_MATCH_INDEX /* EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_TCAM_MATCH /* EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_MATCH_INDEX /* EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_TCAM_MATCH /* EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_MATCH_INDEX /* EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_TCAM_MATCH /* EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_MATCH_INDEX /* EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_TCAM_MATCH /* EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_TCAM_MATCH. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_MATCH_INDEX /* EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_MATCH_INDEX. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */

#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP_MIRROR_SEQUENCE_NUMBER_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP_MIRROR_SEQUENCE_NUMBER_PROFILE_PKT_ADDR /* EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP_MIRROR_SEQUENCE_NUMBER_PROFILE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP_NUMBER_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP_NUMBER_TABLE_PKT_ADDR /* EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP_NUMBER_TABLE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP_PROFILE_PKT_ADDR /* EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP_PROFILE_PKT_ADDR. */

#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP_NEW_SEQUENCE_NUM BCMPKT_TRACE_DOP_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP_NEW_SEQUENCE_NUM /* EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP_NEW_SEQUENCE_NUM. */

#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP_PKT_SEQUENCE_NUM BCMPKT_TRACE_DOP_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP_PKT_SEQUENCE_NUM /* EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP_PKT_SEQUENCE_NUM. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_4 /* FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_4. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_3 /* FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_2 /* FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_1 /* FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_0 /* FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_3 /* FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_2 /* FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_1 /* FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_0 /* FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_4 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_4. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_3 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_2 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_1 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_0 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_4 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_4. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_3 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_2 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_1 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_0 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_4 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_4. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_3 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_3. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_2 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_2. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_1 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_1. */

#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_0 /* FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_0. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP_EGR_FLEX_CTR_BUS BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP_EGR_FLEX_CTR_BUS /* FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP_EGR_FLEX_CTR_BUS. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR. */

#define BCM_PKTIO_TRACE_DOP_FID_COUNT   BCMPKT_TRACE_DOP_FID_COUNT /* TRACE_DOP FIELD ID NUMBER */

#define BCM_PKTIO_TRACE_DOP_ID_FID_INVALID  BCMPKT_TRACE_DOP_ID_FID_INVALID /* Invalid
                                                          BCM_PKTIO_TRACE_DOP_ID
                                                          FID indicator */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER0_HME_STAGE0_DOP BCMPKT_TRACE_DOP_ID_IPARSER0_HME_STAGE0_DOP /* IPARSER0_HME_STAGE0_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA10_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA10_MY_DOP_INDEX_DOP /* MEMDB_IFTA10_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA20_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA20_MY_DOP_INDEX_DOP /* MEMDB_IFTA20_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE0_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE0_DOP /* IPARSER1_HME_STAGE0_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE1_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE1_DOP /* IPARSER1_HME_STAGE1_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE2_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE2_DOP /* IPARSER1_HME_STAGE2_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE3_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE3_DOP /* IPARSER1_HME_STAGE3_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE4_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE4_DOP /* IPARSER1_HME_STAGE4_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE5_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE5_DOP /* IPARSER1_HME_STAGE5_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP /* IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP /* IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP /* IFTA30_E2T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE0_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE0_DOP /* IPARSER2_HME_STAGE0_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE1_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE1_DOP /* IPARSER2_HME_STAGE1_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE2_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE2_DOP /* IPARSER2_HME_STAGE2_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE3_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE3_DOP /* IPARSER2_HME_STAGE3_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE4_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE4_DOP /* IPARSER2_HME_STAGE4_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP /* IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP /* IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP /* IFTA40_E2T_01_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP /* IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP /* IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP /* IFTA40_E2T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP /* IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP /* IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP /* IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP /* IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP /* IFTA40_T4T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP /* IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP /* IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP /* IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP /* IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP /* IFTA40_T4T_01_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP /* MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP /* MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP /* MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFSL40_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL40_LTS_TCAM_KEY_DOP /* IFSL40_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFSL41_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL41_LTS_TCAM_KEY_DOP /* IFSL41_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP /* IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP /* IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP /* IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP /* IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP /* IFTA50_T4T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP /* MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP BCMPKT_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP /* MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP /* MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP /* MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP /* MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP /* MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP /* MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA50_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA50_MY_DOP_INDEX_DOP /* MEMDB_IFTA50_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA60_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA60_MY_DOP_INDEX_DOP /* MEMDB_IFTA60_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP /* FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP /* FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_NORM_FD_NORM_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_NORM_FD_NORM_DOP /* FLEX_DIGEST_NORM_FD_NORM_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_HASH_FD_HASH_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_HASH_FD_HASH_DOP /* FLEX_DIGEST_HASH_FD_HASH_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA70_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA70_MY_DOP_INDEX_DOP /* MEMDB_IFTA70_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFSL70_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL70_LTS_TCAM_KEY_DOP /* IFSL70_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP /* IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP /* IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP /* IFTA80_E2T_01_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP /* IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP /* IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP /* IFTA80_E2T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP /* IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP /* IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP /* IFTA80_E2T_03_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP /* IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP /* IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP /* IFTA80_E2T_02_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP /* IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP /* IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP /* IFTA80_T2T_01_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP /* IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP /* IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP /* IFTA80_T2T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP /* FLEX_QOS_PHB_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP /* FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP /* MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP /* MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP /* IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP /* IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP /* IFTA90_E2T_03_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP /* IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP /* IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP /* IFTA90_E2T_01_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP /* IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP /* IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP /* IFTA90_E2T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP /* IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP /* IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP /* IFTA90_E2T_02_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP /* FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_B_DOP /* FLEX_CTR_ST_ING0_COUNTER_B_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP /* FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_A_DOP /* FLEX_CTR_ST_ING0_COUNTER_A_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFSL90_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL90_LTS_TCAM_KEY_DOP /* IFSL90_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFSL91_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL91_LTS_TCAM_KEY_DOP /* IFSL91_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP /* IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP /* IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP /* IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP /* IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP /* IFTA100_T4T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP /* IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP /* IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP /* IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP /* IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP /* IFTA100_T4T_02_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP /* IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP /* IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP /* IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP /* IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP /* IFTA100_T4T_01_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_2_KEY_DOP /* MEMDB_IFTA100_MEM1_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_2_KEY_DOP /* MEMDB_IFTA100_MEM0_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_2_KEY_DOP /* MEMDB_IFTA100_MEM7_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_2_KEY_DOP /* MEMDB_IFTA100_MEM8_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_0_KEY_DOP /* MEMDB_IFTA100_MEM9_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_3_KEY_DOP /* MEMDB_IFTA100_MEM11_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_3_KEY_DOP /* MEMDB_IFTA100_MEM7_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_0_KEY_DOP /* MEMDB_IFTA100_MEM8_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_2_KEY_DOP /* MEMDB_IFTA100_MEM10_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_0_KEY_DOP /* MEMDB_IFTA100_MEM5_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_2_KEY_DOP /* MEMDB_IFTA100_MEM3_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_3_KEY_DOP /* MEMDB_IFTA100_MEM8_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_0_KEY_DOP /* MEMDB_IFTA100_MEM11_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_1_KEY_DOP /* MEMDB_IFTA100_MEM2_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_3_KEY_DOP /* MEMDB_IFTA100_MEM4_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_2_KEY_DOP /* MEMDB_IFTA100_MEM4_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_3_KEY_DOP /* MEMDB_IFTA100_MEM5_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_1_KEY_DOP /* MEMDB_IFTA100_MEM10_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_1_KEY_DOP /* MEMDB_IFTA100_MEM6_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_3_KEY_DOP /* MEMDB_IFTA100_MEM10_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_1_KEY_DOP /* MEMDB_IFTA100_MEM4_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_2_KEY_DOP /* MEMDB_IFTA100_MEM9_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_0_KEY_DOP /* MEMDB_IFTA100_MEM3_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_0_KEY_DOP /* MEMDB_IFTA100_MEM2_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_0_KEY_DOP /* MEMDB_IFTA100_MEM1_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_1_KEY_DOP /* MEMDB_IFTA100_MEM11_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_2_KEY_DOP /* MEMDB_IFTA100_MEM2_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_1_KEY_DOP /* MEMDB_IFTA100_MEM8_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_1_KEY_DOP /* MEMDB_IFTA100_MEM7_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_3_KEY_DOP /* MEMDB_IFTA100_MEM9_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_0_KEY_DOP /* MEMDB_IFTA100_MEM6_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_2_KEY_DOP /* MEMDB_IFTA100_MEM6_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_1_KEY_DOP /* MEMDB_IFTA100_MEM9_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_1_KEY_DOP /* MEMDB_IFTA100_MEM5_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_3_KEY_DOP /* MEMDB_IFTA100_MEM0_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_3_KEY_DOP /* MEMDB_IFTA100_MEM3_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_0_KEY_DOP /* MEMDB_IFTA100_MEM0_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_1_KEY_DOP /* MEMDB_IFTA100_MEM1_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_0_KEY_DOP /* MEMDB_IFTA100_MEM4_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_1_KEY_DOP /* MEMDB_IFTA100_MEM3_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_3_KEY_DOP /* MEMDB_IFTA100_MEM2_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_0_KEY_DOP /* MEMDB_IFTA100_MEM10_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_3_KEY_DOP /* MEMDB_IFTA100_MEM6_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_1_KEY_DOP /* MEMDB_IFTA100_MEM0_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_2_KEY_DOP /* MEMDB_IFTA100_MEM5_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_0_KEY_DOP /* MEMDB_IFTA100_MEM7_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_3_KEY_DOP /* MEMDB_IFTA100_MEM1_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_2_KEY_DOP /* MEMDB_IFTA100_MEM11_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_INDEX_DOP /* MEMDB_IFTA100_MEM9_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_INDEX_DOP /* MEMDB_IFTA100_MEM10_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_INDEX_DOP /* MEMDB_IFTA100_MEM7_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_INDEX_DOP /* MEMDB_IFTA100_MEM5_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_INDEX_DOP /* MEMDB_IFTA100_MEM8_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_INDEX_DOP /* MEMDB_IFTA100_MEM2_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_INDEX_DOP /* MEMDB_IFTA100_MEM11_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_INDEX_DOP /* MEMDB_IFTA100_MEM6_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_INDEX_DOP /* MEMDB_IFTA100_MEM1_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_INDEX_DOP /* MEMDB_IFTA100_MEM4_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_INDEX_DOP /* MEMDB_IFTA100_MEM3_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_INDEX_DOP /* MEMDB_IFTA100_MEM0_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFSL100_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL100_LTS_TCAM_KEY_DOP /* IFSL100_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP /* ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP /* ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP /* ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA110_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA110_MY_DOP_INDEX_DOP /* MEMDB_IFTA110_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ETRAP_ETR_HIT_RTAG_HASH_DOP BCMPKT_TRACE_DOP_ID_ETRAP_ETR_HIT_RTAG_HASH_DOP /* ETRAP_ETR_HIT_RTAG_HASH_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ETRAP_ETR_OUT_DOP BCMPKT_TRACE_DOP_ID_ETRAP_ETR_OUT_DOP /* ETRAP_ETR_OUT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP BCMPKT_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP /* DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP BCMPKT_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP /* DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP /* ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP /* ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP /* ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA120_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA120_MY_DOP_INDEX_DOP /* MEMDB_IFTA120_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA130_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA130_MY_DOP_INDEX_DOP /* MEMDB_IFTA130_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA140_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA140_MY_DOP_INDEX_DOP /* MEMDB_IFTA140_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP /* FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP /* FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_B_DOP /* FLEX_CTR_ST_ING1_COUNTER_B_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP /* FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_A_DOP /* FLEX_CTR_ST_ING1_COUNTER_A_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP /* IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP /* IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP /* IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP /* IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP /* IFTA150_T4T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_1_KEY_DOP /* MEMDB_IFTA150_MEM0_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_3_KEY_DOP /* MEMDB_IFTA150_MEM0_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_2_KEY_DOP /* MEMDB_IFTA150_MEM0_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_0_KEY_DOP /* MEMDB_IFTA150_MEM0_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP /* MEMDB_IFTA150_MEM0_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP BCMPKT_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP /* IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP /* IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP /* IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP BCMPKT_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP BCMPKT_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP /* IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP /* IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP /* IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP /* IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP /* IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP /* IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP /* IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP /* IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP /* IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP /* IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP /* FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP /* FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_IFSL140_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL140_LTS_TCAM_KEY_DOP /* IFSL140_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP /* EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP /* EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP /* EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP /* EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP /* EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MPB_FIXED_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MPB_FIXED_DOP /* EPRE_EDEV_CONFIG_MPB_FIXED_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_CCBE_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_CCBE_DOP /* EPRE_EDEV_CONFIG_CCBE_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP /* EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP /* EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP /* EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_EFTA10_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_EFTA10_MY_DOP_INDEX_DOP /* MEMDB_EFTA10_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_EFTA20_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_EFTA20_MY_DOP_INDEX_DOP /* MEMDB_EFTA20_MY_DOP_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFSL20_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EFSL20_LTS_TCAM_KEY_DOP /* EFSL20_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP /* EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP /* EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP /* EFTA30_E2T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP /* EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP /* EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP /* EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP /* EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP /* EFTA30_T4T_00_LTS_TCAM_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP BCMPKT_TRACE_DOP_ID_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP /* EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP BCMPKT_TRACE_DOP_ID_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP /* EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP /* FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP /* FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP /* FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP /* FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP /* FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_B_DOP /* FLEX_CTR_ST_EGR_COUNTER_B_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP /* FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_A_DOP /* FLEX_CTR_ST_EGR_COUNTER_A_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP /* MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP /* MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP /* MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP /* MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP /* MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EFSL30_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EFSL30_LTS_TCAM_KEY_DOP /* EFSL30_LTS_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP BCMPKT_TRACE_DOP_ID_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP /* EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP /* QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP /* QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP /* QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP /* EDIT_CTRL_ZONE_4_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP /* EDIT_CTRL_ZONE_3_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP /* EDIT_CTRL_ZONE_1_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP /* EDIT_CTRL_ZONE_2_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP /* EDIT_CTRL_ZONE_0_TCAM_KEY_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP /* EDIT_CTRL_ZONE_0_TCAM_HIT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP /* EDIT_CTRL_ZONE_4_TCAM_HIT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP /* EDIT_CTRL_ZONE_3_TCAM_HIT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP /* EDIT_CTRL_ZONE_2_TCAM_HIT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP /* EDIT_CTRL_ZONE_1_TCAM_HIT_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP /* EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP /* EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP /* EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP /* EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP /* EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP /* EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_EDITOR_MATCH_ID_DOP BCMPKT_TRACE_DOP_ID_FLEX_EDITOR_MATCH_ID_DOP /* FLEX_EDITOR_MATCH_ID_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_EDITOR_VHLEN_DOP BCMPKT_TRACE_DOP_ID_FLEX_EDITOR_VHLEN_DOP /* FLEX_EDITOR_VHLEN_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_EDITOR_EDIT_ID_DOP BCMPKT_TRACE_DOP_ID_FLEX_EDITOR_EDIT_ID_DOP /* FLEX_EDITOR_EDIT_ID_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP /* FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP /* FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP. */

#define BCM_PKTIO_TRACE_DOP_ID_COUNT    BCMPKT_TRACE_DOP_ID_COUNT /* TRACE_DOP_ID FIELD ID
                                                      NUMBER */

#endif /* defined(INCLUDE_PKTIO) || defined(BCM_LTSW_SUPPORT) */

#endif /* __BCM_PKTIO_DEFS_H__ */
