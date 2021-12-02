/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_PKTIO_DEFS_H__
#define __BCM_PKTIO_DEFS_H__

#if defined(INCLUDE_PKTIO) || defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT)

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

#define BCM_PKTIO_RXPMD_MATCH_ID_LO BCMPKT_RXPMD_MATCH_ID_LO /* Low 32 bits of the packet
                                                  MATCH_ID. */

#define BCM_PKTIO_RXPMD_MATCH_ID_HI BCMPKT_RXPMD_MATCH_ID_HI /* High 18 bits of the packet
                                                  MATCH_ID. */

#define BCM_PKTIO_RXPMD_MPB_FLEX_DATA_TYPE  BCMPKT_RXPMD_MPB_FLEX_DATA_TYPE /* MPB flex data_type
                                                          from
                                                          MPB_FIXED.flex_data_type */

#define BCM_PKTIO_RXPMD_INT_CN  BCMPKT_RXPMD_INT_CN /* From EPRE/EDEV_CONFIG table
                                              EGR_INT_CN_UPDPATE.int_cn */

#define BCM_PKTIO_RXPMD_CNG     BCMPKT_RXPMD_CNG /* From MPB_FIXED */

#define BCM_PKTIO_RXPMD_EGR_ZONE_REMAP_CTRL BCMPKT_RXPMD_EGR_ZONE_REMAP_CTRL /* From MPB_FIXED */

#define BCM_PKTIO_RXPMD_DMA_HEADER_VERSION  BCMPKT_RXPMD_DMA_HEADER_VERSION /* Constant specified in
                                                          NPL */

#define BCM_PKTIO_RXPMD_MULTICAST   BCMPKT_RXPMD_MULTICAST /* Multicast packet from
                                                  MPB_FIXED */

#define BCM_PKTIO_RXPMD_COPY_TO_CPU BCMPKT_RXPMD_COPY_TO_CPU /* Copy to CPU from MPB_FIXED */

#define BCM_PKTIO_RXPMD_TRUNCATE_CPU_COPY   BCMPKT_RXPMD_TRUNCATE_CPU_COPY /* Trucate CPU copy from
                                                          MPB_FIXED */

#define BCM_PKTIO_RXPMD_INCOMING_INT_HDR_TYPE BCMPKT_RXPMD_INCOMING_INT_HDR_TYPE /* Incoming INT header
                                                          type. */

#define BCM_PKTIO_RXPMD_DOP_TRIGGER BCMPKT_RXPMD_DOP_TRIGGER /* MPB_FIXED.dop_trigger. */

#define BCM_PKTIO_RXPMD_EPARSE_EXTRACT_OFFSETS_3_0_OR_MIRROR_ENCAP_INDEX BCMPKT_RXPMD_EPARSE_EXTRACT_OFFSETS_3_0_OR_MIRROR_ENCAP_INDEX /* From EPRE. If mirror
                                                          pkt then ==
                                                          MIRROR_ENCAP_INDEx
                                                          else ==
                                                          eparse_extract_offsets[3:0]. */

#define BCM_PKTIO_RXPMD_EPARSE_EXTRACT_OFFSETS_7_4 BCMPKT_RXPMD_EPARSE_EXTRACT_OFFSETS_7_4 /* Eparse_extract_offsets[7:4]. */

#define BCM_PKTIO_RXPMD_ING_L3_INTF_VALID   BCMPKT_RXPMD_ING_L3_INTF_VALID /* Valid for non-BFD
                                                          packet. */

#define BCM_PKTIO_RXPMD_O_NHI   BCMPKT_RXPMD_O_NHI /* Overlay next hop. */

#define BCM_PKTIO_RXPMD_IEU     BCMPKT_RXPMD_IEU /* Inner Entropy Used indication. */

#define BCM_PKTIO_RXPMD_IEU_VALID   BCMPKT_RXPMD_IEU_VALID /* IEU indication is valid. */

#define BCM_PKTIO_RXPMD_INCOMING_OPAQUE_TAG_STATUS BCMPKT_RXPMD_INCOMING_OPAQUE_TAG_STATUS /* Incoming opaque tag
                                                          status indication. */

#define BCM_PKTIO_RXPMD_EPARSE_EXTRACT_OFFSETS_6_4 BCMPKT_RXPMD_EPARSE_EXTRACT_OFFSETS_6_4 /* Eparse_extract_offsets[6:4]. */

#define BCM_PKTIO_RXPMD_ING_PP_PORT BCMPKT_RXPMD_ING_PP_PORT /* From
                                                  EDEV_CONFIG.SRC_SUBPORT_TABLE. */

#define BCM_PKTIO_RXPMD_ARC_ID_LO   BCMPKT_RXPMD_ARC_ID_LO /* Low 32 bits of the packet
                                                  ARC_ID. */

#define BCM_PKTIO_RXPMD_ARC_ID_HI   BCMPKT_RXPMD_ARC_ID_HI /* High 18 bits of the packet
                                                  ARC_ID. */

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

#define BCM_PKTIO_RX_REASON_NH_PROTO_STATUS_DOWN BCMPKT_RX_REASON_NH_PROTO_STATUS_DOWN /* Protection status is
                                                          down. */

#define BCM_PKTIO_RX_REASON_CPU_INVALID_REASON BCMPKT_RX_REASON_CPU_INVALID_REASON /* Invalid opcode. */

#define BCM_PKTIO_RX_REASON_MPLS_TTL_CHECK  BCMPKT_RX_REASON_MPLS_TTL_CHECK /* MPLS TTL Check. */

#define BCM_PKTIO_RX_REASON_SVTAG_CPU_BIT_SET BCMPKT_RX_REASON_SVTAG_CPU_BIT_SET /* Bit Definitions of the
                                                          CPU Opcodes. */

#define BCM_PKTIO_RX_REASON_SUBPORT_ID_LOOKUP_MISS BCMPKT_RX_REASON_SUBPORT_ID_LOOKUP_MISS /* Subport ID lookup miss
                                                          in Channelization. */

#define BCM_PKTIO_RX_REASON_OAM_PROCESSING  BCMPKT_RX_REASON_OAM_PROCESSING /* OAM operation. */

#define BCM_PKTIO_RX_REASON_OAM_PROCESSING_INVALID BCMPKT_RX_REASON_OAM_PROCESSING_INVALID /* OAM invalid. */

#define BCM_PKTIO_RX_REASON_OAM_PROCESSING_OAM_CCM BCMPKT_RX_REASON_OAM_PROCESSING_OAM_CCM /* OAM CCM. */

#define BCM_PKTIO_RX_REASON_OAM_PROCESSING_OAM_LM BCMPKT_RX_REASON_OAM_PROCESSING_OAM_LM /* OAM LM */

#define BCM_PKTIO_RX_REASON_OAM_PROCESSING_OAM_DM BCMPKT_RX_REASON_OAM_PROCESSING_OAM_DM /* OAM DM. */

#define BCM_PKTIO_RX_REASON_OAM_PROCESSING_OAM_OTHER_OPCODES BCMPKT_RX_REASON_OAM_PROCESSING_OAM_OTHER_OPCODES /* OAM Other opcodes. */

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

#define BCM_PKTIO_TXPMD_UNICAST_PKT BCMPKT_TXPMD_UNICAST_PKT /* Will set CCBI_B.UNICAST_PKT */

#define BCM_PKTIO_TXPMD_TS_ACTION_LSB   BCMPKT_TXPMD_TS_ACTION_LSB /* Timestamp action LSB */

#define BCM_PKTIO_TXPMD_TS_ACTION_MSB   BCMPKT_TXPMD_TS_ACTION_MSB /* Timestamp action MSB */

#define BCM_PKTIO_TXPMD_TS_TYPE BCMPKT_TXPMD_TS_TYPE /* Time statmp type.  Encodings are
                                              0-NTP TOD(64 bit), 1-PTP
                                              TOD(64bit) */

#define BCM_PKTIO_TXPMD_DST_SUBPORT_NUM BCMPKT_TXPMD_DST_SUBPORT_NUM /* Destination subport number */

#define BCM_PKTIO_TXPMD_UDP_CHECKSUM_UPDATE_ENABLE BCMPKT_TXPMD_UDP_CHECKSUM_UPDATE_ENABLE /* Enable UDP incremental
                                                          checksum */

#define BCM_PKTIO_TXPMD_UDP_CHECKSUM_OFFSET BCMPKT_TXPMD_UDP_CHECKSUM_OFFSET /* Offset to UDP checksum
                                                          field from start of
                                                          MACS-SA */

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

#define BCM_PKTIO_TRACE_DVP     BCMPKT_TRACE_DVP /* DVP. */

#define BCM_PKTIO_TRACE_SVP     BCMPKT_TRACE_SVP /* SVP. */

#define BCM_PKTIO_TRACE_INNER_L2_OUTER_TAGGED BCMPKT_TRACE_INNER_L2_OUTER_TAGGED /* Inner L2 has a vlan
                                                          tag. */

#define BCM_PKTIO_TRACE_O_NEXT_HOP  BCMPKT_TRACE_O_NEXT_HOP /* Overlay Next Hop. */

#define BCM_PKTIO_TRACE_VFI     BCMPKT_TRACE_VFI /* VFI. */

#define BCM_PKTIO_TRACE_MY_STATION_2_HIT    BCMPKT_TRACE_MY_STATION_2_HIT /* Indicates
                                                          MY_STATION_2_TCAM
                                                          lookup hit. */

#define BCM_PKTIO_TRACE_MY_STATION_2_INDEX  BCMPKT_TRACE_MY_STATION_2_INDEX /* Index for
                                                          MY_STATION_2_TCAM when
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

#define BCM_PKTIO_TRACE_DROP_REASON_ADAPT_MISS_DROP BCMPKT_TRACE_DROP_REASON_ADAPT_MISS_DROP /* Adapt miss drop. */

#define BCM_PKTIO_TRACE_DROP_REASON_OVERLAY_UNDERLAY_RANGE_ERROR BCMPKT_TRACE_DROP_REASON_OVERLAY_UNDERLAY_RANGE_ERROR /* Both O_NH/INTF and
                                                          U_NH_INTF are valid,
                                                          and each of these
                                                          indices do NOT comply
                                                          with configured bank
                                                          allocation bitmap. */

#define BCM_PKTIO_TRACE_DROP_REASON_VXLAN_SIP_LOOKUP_MISS_DROP BCMPKT_TRACE_DROP_REASON_VXLAN_SIP_LOOKUP_MISS_DROP /* VXLAN SIP lookup miss
                                                          drop. */

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

#define BCM_PKTIO_TRACE_DOP_FID_INVALID     BCMPKT_TRACE_DOP_FID_INVALID /* <-1> Invalid
                                                          BCMPKT_TRACE_DOP FID
                                                          indicator */
#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR /* <0>
                                                          IPARSER0_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH /* <1>
                                                          IPARSER0_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT /* <2>
                                                          IPARSER0_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_PKT_DATA BCMPKT_TRACE_DOP_IPARSER0_HME_STAGE0_DOP_STAGE0_PKT_DATA /* <3>
                                                          IPARSER0_HME_STAGE0_DOP_STAGE0_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <4>
                                                          MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <5>
                                                          MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <6>
                                                          MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <7>
                                                          MEMDB_IFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFTA10_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA10_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <8>
                                                          IFTA10_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <9>
                                                          IFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <10>
                                                          MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <11>
                                                          MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <12>
                                                          MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <13>
                                                          MEMDB_IFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFTA20_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA20_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <14>
                                                          IFTA20_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <15>
                                                          IFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR /* <16>
                                                          IPARSER1_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH /* <17>
                                                          IPARSER1_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT /* <18>
                                                          IPARSER1_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE0_DOP_STAGE0_PKT_DATA /* <19>
                                                          IPARSER1_HME_STAGE0_DOP_STAGE0_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR /* <20>
                                                          IPARSER1_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH /* <21>
                                                          IPARSER1_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT /* <22>
                                                          IPARSER1_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE1_DOP_STAGE1_PKT_DATA /* <23>
                                                          IPARSER1_HME_STAGE1_DOP_STAGE1_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR /* <24>
                                                          IPARSER1_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH /* <25>
                                                          IPARSER1_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT /* <26>
                                                          IPARSER1_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE2_DOP_STAGE2_PKT_DATA /* <27>
                                                          IPARSER1_HME_STAGE2_DOP_STAGE2_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR /* <28>
                                                          IPARSER1_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH /* <29>
                                                          IPARSER1_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT /* <30>
                                                          IPARSER1_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE3_DOP_STAGE3_PKT_DATA /* <31>
                                                          IPARSER1_HME_STAGE3_DOP_STAGE3_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR /* <32>
                                                          IPARSER1_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH /* <33>
                                                          IPARSER1_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT /* <34>
                                                          IPARSER1_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE4_DOP_STAGE4_PKT_DATA /* <35>
                                                          IPARSER1_HME_STAGE4_DOP_STAGE4_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_HFE_PROFILE_PTR /* <36>
                                                          IPARSER1_HME_STAGE5_DOP_STAGE5_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_TCAM_TCAM_MATCH /* <37>
                                                          IPARSER1_HME_STAGE5_DOP_STAGE5_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_SHIFT_AMOUNT /* <38>
                                                          IPARSER1_HME_STAGE5_DOP_STAGE5_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_PKT_DATA BCMPKT_TRACE_DOP_IPARSER1_HME_STAGE5_DOP_STAGE5_PKT_DATA /* <39>
                                                          IPARSER1_HME_STAGE5_DOP_STAGE5_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <40>
                                                          IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <41>
                                                          IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <42>
                                                          IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <43>
                                                          IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <44>
                                                          IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <45>
                                                          IFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <46>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <47>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <48>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <49>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <50>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <51>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <52>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <53>
                                                          IFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR /* <54>
                                                          IPARSER2_HME_STAGE0_DOP_STAGE0_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH /* <55>
                                                          IPARSER2_HME_STAGE0_DOP_STAGE0_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT /* <56>
                                                          IPARSER2_HME_STAGE0_DOP_STAGE0_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE0_DOP_STAGE0_PKT_DATA /* <57>
                                                          IPARSER2_HME_STAGE0_DOP_STAGE0_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR /* <58>
                                                          IPARSER2_HME_STAGE1_DOP_STAGE1_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH /* <59>
                                                          IPARSER2_HME_STAGE1_DOP_STAGE1_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT /* <60>
                                                          IPARSER2_HME_STAGE1_DOP_STAGE1_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE1_DOP_STAGE1_PKT_DATA /* <61>
                                                          IPARSER2_HME_STAGE1_DOP_STAGE1_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR /* <62>
                                                          IPARSER2_HME_STAGE2_DOP_STAGE2_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH /* <63>
                                                          IPARSER2_HME_STAGE2_DOP_STAGE2_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT /* <64>
                                                          IPARSER2_HME_STAGE2_DOP_STAGE2_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE2_DOP_STAGE2_PKT_DATA /* <65>
                                                          IPARSER2_HME_STAGE2_DOP_STAGE2_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR /* <66>
                                                          IPARSER2_HME_STAGE3_DOP_STAGE3_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH /* <67>
                                                          IPARSER2_HME_STAGE3_DOP_STAGE3_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT /* <68>
                                                          IPARSER2_HME_STAGE3_DOP_STAGE3_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE3_DOP_STAGE3_PKT_DATA /* <69>
                                                          IPARSER2_HME_STAGE3_DOP_STAGE3_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR /* <70>
                                                          IPARSER2_HME_STAGE4_DOP_STAGE4_HFE_PROFILE_PTR. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH /* <71>
                                                          IPARSER2_HME_STAGE4_DOP_STAGE4_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT /* <72>
                                                          IPARSER2_HME_STAGE4_DOP_STAGE4_SHIFT_AMOUNT. */
#define BCM_PKTIO_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_PKT_DATA BCMPKT_TRACE_DOP_IPARSER2_HME_STAGE4_DOP_STAGE4_PKT_DATA /* <73>
                                                          IPARSER2_HME_STAGE4_DOP_STAGE4_PKT_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <74>
                                                          IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <75>
                                                          IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <76>
                                                          IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <77>
                                                          IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <78>
                                                          IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <79>
                                                          IFTA40_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <80>
                                                          IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <81>
                                                          IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <82>
                                                          IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <83>
                                                          IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <84>
                                                          IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <85>
                                                          IFTA40_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <86>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <87>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <88>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <89>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <90>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <91>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <92>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA40_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <93>
                                                          IFTA40_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <94>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <95>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <96>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <97>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <98>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <99>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <100>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA40_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <101>
                                                          IFTA40_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <102>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <103>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <104>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <105>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <106>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <107>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <108>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA40_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <109>
                                                          IFTA40_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <110>
                                                          IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <111>
                                                          IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <112>
                                                          IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <113>
                                                          IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <114>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <115>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <116>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <117>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <118>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <119>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <120>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <121>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <122>
                                                          IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <123>
                                                          IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <124>
                                                          IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <125>
                                                          IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <126>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <127>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <128>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <129>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <130>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <131>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <132>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <133>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <134>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <135>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <136>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <137>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <138>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <139>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <140>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <141>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <142>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <143>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <144>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <145>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <146>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA40_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <147>
                                                          IFTA40_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <148>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <149>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <150>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <151>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <152>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <153>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <154>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <155>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <156>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <157>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <158>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <159>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <160>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA40_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA40_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <161>
                                                          IFTA40_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_KEY /* <162>
                                                          MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* <163>
                                                          MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_KEY /* <164>
                                                          MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_PKT_RD /* <165>
                                                          MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP_MEM1_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_KEY /* <166>
                                                          MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* <167>
                                                          MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_KEY /* <168>
                                                          MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* <169>
                                                          MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_KEY /* <170>
                                                          MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_PKT_RD /* <171>
                                                          MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP_MEM1_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_KEY /* <172>
                                                          MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_PKT_RD /* <173>
                                                          MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP_MEM1_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_KEY /* <174>
                                                          MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* <175>
                                                          MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_KEY /* <176>
                                                          MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_PKT_RD /* <177>
                                                          MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP_MEM1_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* <178>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* <179>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* <180>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* <181>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* <182>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* <183>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* <184>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* <185>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX /* <186>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH /* <187>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX /* <188>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH /* <189>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX /* <190>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH /* <191>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX /* <192>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH /* <193>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <194>
                                                          IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <195>
                                                          IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <196>
                                                          IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <197>
                                                          IFSL40_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <198>
                                                          IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <199>
                                                          IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <200>
                                                          IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <201>
                                                          IFSL41_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <202>
                                                          IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <203>
                                                          IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <204>
                                                          IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <205>
                                                          IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <206>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <207>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <208>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <209>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <210>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <211>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <212>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <213>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA50_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <214>
                                                          IFTA50_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA50_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <215>
                                                          IFTA50_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA50_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <216>
                                                          IFTA50_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA50_I1T_01_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA50_I1T_01_INDEX_DOP_LKP0_LTPR_WIN /* <217>
                                                          IFTA50_I1T_01_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP_TCAM_ONLY_KEY BCMPKT_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP_TCAM_ONLY_KEY /* <218>
                                                          MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP_TCAM_ONLY_KEY. */
#define BCM_PKTIO_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_MATCH_INDEX BCMPKT_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_MATCH_INDEX /* <219>
                                                          MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_TCAM_MATCH BCMPKT_TRACE_DOP_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_TCAM_MATCH /* <220>
                                                          MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP_TCAM_ONLY_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_KEY /* <221>
                                                          MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* <222>
                                                          MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_KEY /* <223>
                                                          MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* <224>
                                                          MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_KEY /* <225>
                                                          MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* <226>
                                                          MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_KEY /* <227>
                                                          MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* <228>
                                                          MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* <229>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* <230>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* <231>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* <232>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* <233>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* <234>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* <235>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* <236>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <237>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <238>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <239>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <240>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* <241>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* <242>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* <243>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* <244>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <245>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <246>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <247>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <248>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* <249>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* <250>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* <251>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* <252>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* <253>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* <254>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* <255>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* <256>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA60_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <257>
                                                          IFTA60_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA60_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <258>
                                                          IFTA60_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA60_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <259>
                                                          IFTA60_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_I1T_01_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA60_I1T_01_INDEX_DOP_LKP0_LTPR_WIN /* <260>
                                                          IFTA60_I1T_01_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA60_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <261>
                                                          IFTA60_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_I1T_02_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA60_I1T_02_INDEX_DOP_LKP0_LTPR_WIN /* <262>
                                                          IFTA60_I1T_02_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <263>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <264>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <265>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <266>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <267>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <268>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <269>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <270>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <271>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <272>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <273>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <274>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <275>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA60_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA60_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <276>
                                                          IFTA60_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP_MEM0_1_KEY /* <277>
                                                          MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* <278>
                                                          MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_1_KEY /* <279>
                                                          MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_1_PKT_RD /* <280>
                                                          MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_1_KEY /* <281>
                                                          MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_1_PKT_RD /* <282>
                                                          MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_1_KEY /* <283>
                                                          MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_1_PKT_RD /* <284>
                                                          MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* <285>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* <286>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_TCAM_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_TCAM_INDEX /* <287>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_0_TCAM_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* <288>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_TCAM_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_TCAM_INDEX /* <289>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_TCAM_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* <290>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* <291>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_TCAM_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_TCAM_INDEX /* <292>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_TCAM_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* <293>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* <294>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_TCAM_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_TCAM_INDEX /* <295>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_TCAM_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* <296>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCG. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_3_KEY /* <297>
                                                          MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* <298>
                                                          MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_0_KEY /* <299>
                                                          MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* <300>
                                                          MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_2_KEY /* <301>
                                                          MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* <302>
                                                          MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP_NET_LAYER_TCAM_0_KEY BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP_NET_LAYER_TCAM_0_KEY /* <303>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP_NET_LAYER_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_MATCH_INDEX /* <304>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_MATCH_INDEX /* <305>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_MATCH_INDEX /* <306>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_TCAM_MATCH /* <307>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_TCAM_MATCH /* <308>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_TCAM_MATCH /* <309>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP_NET_LAYER_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_NORM_FD_NORM_DOP_OUT_FD_NORM_FLD_BUS BCMPKT_TRACE_DOP_FLEX_DIGEST_NORM_FD_NORM_DOP_OUT_FD_NORM_FLD_BUS /* <310>
                                                          FLEX_DIGEST_NORM_FD_NORM_DOP_OUT_FD_NORM_FLD_BUS. */
#define BCM_PKTIO_TRACE_DOP_FLEX_DIGEST_HASH_FD_HASH_DOP_OUT_ING_HASH_BUS BCMPKT_TRACE_DOP_FLEX_DIGEST_HASH_FD_HASH_DOP_OUT_ING_HASH_BUS /* <311>
                                                          FLEX_DIGEST_HASH_FD_HASH_DOP_OUT_ING_HASH_BUS. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <312>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <313>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <314>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <315>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* <316>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* <317>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* <318>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* <319>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFTA70_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA70_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <320>
                                                          IFTA70_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA70_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA70_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <321>
                                                          IFTA70_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA70_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA70_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <322>
                                                          IFTA70_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA70_I1T_01_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA70_I1T_01_INDEX_DOP_LKP0_LTPR_WIN /* <323>
                                                          IFTA70_I1T_01_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <324>
                                                          IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <325>
                                                          IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <326>
                                                          IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <327>
                                                          IFSL70_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <328>
                                                          IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <329>
                                                          IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <330>
                                                          IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <331>
                                                          IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <332>
                                                          IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <333>
                                                          IFTA80_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <334>
                                                          IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <335>
                                                          IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <336>
                                                          IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <337>
                                                          IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <338>
                                                          IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <339>
                                                          IFTA80_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <340>
                                                          IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <341>
                                                          IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <342>
                                                          IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <343>
                                                          IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <344>
                                                          IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <345>
                                                          IFTA80_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <346>
                                                          IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <347>
                                                          IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <348>
                                                          IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <349>
                                                          IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <350>
                                                          IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <351>
                                                          IFTA80_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <352>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <353>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <354>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <355>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <356>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <357>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <358>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA80_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <359>
                                                          IFTA80_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <360>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <361>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <362>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <363>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <364>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <365>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <366>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA80_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <367>
                                                          IFTA80_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <368>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <369>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <370>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <371>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <372>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <373>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <374>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA80_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <375>
                                                          IFTA80_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <376>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <377>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <378>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <379>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <380>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <381>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <382>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA80_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <383>
                                                          IFTA80_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <384>
                                                          IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <385>
                                                          IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <386>
                                                          IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <387>
                                                          IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <388>
                                                          IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <389>
                                                          IFTA80_T2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <390>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <391>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <392>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <393>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <394>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <395>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <396>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <397>
                                                          IFTA80_T2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <398>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <399>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <400>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <401>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <402>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <403>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <404>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA80_T2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <405>
                                                          IFTA80_T2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <406>
                                                          IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <407>
                                                          IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <408>
                                                          IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <409>
                                                          IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <410>
                                                          IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <411>
                                                          IFTA80_T2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <412>
                                                          FLEX_QOS_PHB_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX /* <413>
                                                          FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH /* <414>
                                                          FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA /* <415>
                                                          FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX BCMPKT_TRACE_DOP_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX /* <416>
                                                          FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <417>
                                                          FLEX_QOS_PHB2_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH /* <418>
                                                          FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA BCMPKT_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA /* <419>
                                                          FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA. */
#define BCM_PKTIO_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX BCMPKT_TRACE_DOP_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX /* <420>
                                                          FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_KEY /* <421>
                                                          MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_PKT_RD /* <422>
                                                          MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP_MEM2_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_KEY /* <423>
                                                          MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_PKT_RD /* <424>
                                                          MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP_MEM1_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_KEY /* <425>
                                                          MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_PKT_RD /* <426>
                                                          MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP_MEM7_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_KEY /* <427>
                                                          MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_PKT_RD /* <428>
                                                          MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP_MEM6_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_KEY /* <429>
                                                          MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_PKT_RD /* <430>
                                                          MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP_MEM5_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_KEY /* <431>
                                                          MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_PKT_RD /* <432>
                                                          MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP_MEM2_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_KEY /* <433>
                                                          MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_PKT_RD /* <434>
                                                          MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP_MEM5_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_KEY /* <435>
                                                          MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* <436>
                                                          MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_KEY /* <437>
                                                          MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_PKT_RD /* <438>
                                                          MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP_MEM6_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_KEY /* <439>
                                                          MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_PKT_RD /* <440>
                                                          MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP_MEM1_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_KEY /* <441>
                                                          MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_PKT_RD /* <442>
                                                          MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP_MEM4_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_KEY /* <443>
                                                          MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_PKT_RD /* <444>
                                                          MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP_MEM3_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_KEY /* <445>
                                                          MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* <446>
                                                          MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_KEY /* <447>
                                                          MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_PKT_RD /* <448>
                                                          MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP_MEM4_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_KEY /* <449>
                                                          MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_PKT_RD /* <450>
                                                          MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP_MEM7_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_KEY /* <451>
                                                          MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_PKT_RD /* <452>
                                                          MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP_MEM3_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX /* <453>
                                                          MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH /* <454>
                                                          MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX /* <455>
                                                          MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH /* <456>
                                                          MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX /* <457>
                                                          MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH /* <458>
                                                          MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX /* <459>
                                                          MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH /* <460>
                                                          MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX /* <461>
                                                          MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH /* <462>
                                                          MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX /* <463>
                                                          MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH /* <464>
                                                          MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX /* <465>
                                                          MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH /* <466>
                                                          MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX /* <467>
                                                          MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH /* <468>
                                                          MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX /* <469>
                                                          MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH /* <470>
                                                          MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX /* <471>
                                                          MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH /* <472>
                                                          MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX /* <473>
                                                          MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH /* <474>
                                                          MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX /* <475>
                                                          MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH /* <476>
                                                          MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* <477>
                                                          MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* <478>
                                                          MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* <479>
                                                          MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* <480>
                                                          MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX /* <481>
                                                          MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH /* <482>
                                                          MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX /* <483>
                                                          MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH /* <484>
                                                          MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <485>
                                                          IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <486>
                                                          IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <487>
                                                          IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <488>
                                                          IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <489>
                                                          IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <490>
                                                          IFTA90_E2T_03_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <491>
                                                          IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <492>
                                                          IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <493>
                                                          IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <494>
                                                          IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <495>
                                                          IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <496>
                                                          IFTA90_E2T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <497>
                                                          IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <498>
                                                          IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <499>
                                                          IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <500>
                                                          IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <501>
                                                          IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <502>
                                                          IFTA90_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <503>
                                                          IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <504>
                                                          IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <505>
                                                          IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <506>
                                                          IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <507>
                                                          IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <508>
                                                          IFTA90_E2T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <509>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <510>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <511>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <512>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <513>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <514>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <515>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA90_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <516>
                                                          IFTA90_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <517>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <518>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <519>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <520>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <521>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <522>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <523>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA90_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <524>
                                                          IFTA90_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <525>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <526>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <527>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <528>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <529>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <530>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <531>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA90_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <532>
                                                          IFTA90_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <533>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <534>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <535>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <536>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <537>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <538>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <539>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA90_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA90_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <540>
                                                          IFTA90_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* <541>
                                                          FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* <542>
                                                          FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* <543>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* <544>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* <545>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* <546>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B0 /* <547>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B1 /* <548>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B2 /* <549>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B3 /* <550>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_COUNTER_B3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B0 /* <551>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B1 /* <552>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B2 /* <553>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B3 /* <554>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP_BUS_COUNTER_B3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* <555>
                                                          FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* <556>
                                                          FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* <557>
                                                          FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* <558>
                                                          FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 /* <559>
                                                          FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 /* <560>
                                                          FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 /* <561>
                                                          FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 /* <562>
                                                          FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A0 /* <563>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A1 /* <564>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A2 /* <565>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A3 /* <566>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_COUNTER_A3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A0 /* <567>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A1 /* <568>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A2 /* <569>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A3 /* <570>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP_BUS_COUNTER_A3. */
#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <571>
                                                          IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <572>
                                                          IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <573>
                                                          IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <574>
                                                          IFSL90_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <575>
                                                          IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <576>
                                                          IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <577>
                                                          IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <578>
                                                          IFSL91_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <579>
                                                          IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <580>
                                                          IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <581>
                                                          IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <582>
                                                          IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <583>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <584>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <585>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <586>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <587>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <588>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <589>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <590>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <591>
                                                          IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <592>
                                                          IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <593>
                                                          IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <594>
                                                          IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <595>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <596>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <597>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <598>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <599>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <600>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <601>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <602>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <603>
                                                          IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <604>
                                                          IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <605>
                                                          IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <606>
                                                          IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <607>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <608>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <609>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <610>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <611>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <612>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <613>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <614>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <615>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <616>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <617>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <618>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <619>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <620>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <621>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <622>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <623>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <624>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <625>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <626>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <627>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA100_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <628>
                                                          IFTA100_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <629>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <630>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <631>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <632>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <633>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <634>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <635>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <636>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <637>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <638>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <639>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <640>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <641>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA100_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <642>
                                                          IFTA100_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <643>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <644>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <645>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <646>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <647>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <648>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <649>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <650>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <651>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <652>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <653>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <654>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <655>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA100_T4T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA100_T4T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <656>
                                                          IFTA100_T4T_02_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_KEY /* <657>
                                                          MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_PKT_RD /* <658>
                                                          MEMDB_IFTA100_MEM1_2_KEY_DOP_MEM1_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_KEY /* <659>
                                                          MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* <660>
                                                          MEMDB_IFTA100_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_KEY /* <661>
                                                          MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_PKT_RD /* <662>
                                                          MEMDB_IFTA100_MEM7_2_KEY_DOP_MEM7_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_KEY /* <663>
                                                          MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_PKT_RD /* <664>
                                                          MEMDB_IFTA100_MEM8_2_KEY_DOP_MEM8_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_KEY /* <665>
                                                          MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_PKT_RD /* <666>
                                                          MEMDB_IFTA100_MEM9_0_KEY_DOP_MEM9_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_KEY /* <667>
                                                          MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_PKT_RD /* <668>
                                                          MEMDB_IFTA100_MEM11_3_KEY_DOP_MEM11_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_KEY /* <669>
                                                          MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_PKT_RD /* <670>
                                                          MEMDB_IFTA100_MEM7_3_KEY_DOP_MEM7_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_KEY /* <671>
                                                          MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_PKT_RD /* <672>
                                                          MEMDB_IFTA100_MEM8_0_KEY_DOP_MEM8_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_KEY /* <673>
                                                          MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_PKT_RD /* <674>
                                                          MEMDB_IFTA100_MEM10_2_KEY_DOP_MEM10_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_KEY /* <675>
                                                          MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_PKT_RD /* <676>
                                                          MEMDB_IFTA100_MEM5_0_KEY_DOP_MEM5_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_KEY /* <677>
                                                          MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_PKT_RD /* <678>
                                                          MEMDB_IFTA100_MEM3_2_KEY_DOP_MEM3_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_KEY /* <679>
                                                          MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_PKT_RD /* <680>
                                                          MEMDB_IFTA100_MEM8_3_KEY_DOP_MEM8_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_KEY /* <681>
                                                          MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_PKT_RD /* <682>
                                                          MEMDB_IFTA100_MEM11_0_KEY_DOP_MEM11_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_KEY /* <683>
                                                          MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_PKT_RD /* <684>
                                                          MEMDB_IFTA100_MEM2_1_KEY_DOP_MEM2_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_KEY /* <685>
                                                          MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_PKT_RD /* <686>
                                                          MEMDB_IFTA100_MEM4_3_KEY_DOP_MEM4_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_KEY /* <687>
                                                          MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_PKT_RD /* <688>
                                                          MEMDB_IFTA100_MEM4_2_KEY_DOP_MEM4_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_KEY /* <689>
                                                          MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_PKT_RD /* <690>
                                                          MEMDB_IFTA100_MEM5_3_KEY_DOP_MEM5_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_KEY /* <691>
                                                          MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_PKT_RD /* <692>
                                                          MEMDB_IFTA100_MEM10_1_KEY_DOP_MEM10_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_KEY /* <693>
                                                          MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_PKT_RD /* <694>
                                                          MEMDB_IFTA100_MEM6_1_KEY_DOP_MEM6_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_KEY /* <695>
                                                          MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_PKT_RD /* <696>
                                                          MEMDB_IFTA100_MEM10_3_KEY_DOP_MEM10_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_KEY /* <697>
                                                          MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_PKT_RD /* <698>
                                                          MEMDB_IFTA100_MEM4_1_KEY_DOP_MEM4_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_KEY /* <699>
                                                          MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_PKT_RD /* <700>
                                                          MEMDB_IFTA100_MEM9_2_KEY_DOP_MEM9_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_KEY /* <701>
                                                          MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_PKT_RD /* <702>
                                                          MEMDB_IFTA100_MEM3_0_KEY_DOP_MEM3_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_KEY /* <703>
                                                          MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_PKT_RD /* <704>
                                                          MEMDB_IFTA100_MEM2_0_KEY_DOP_MEM2_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_KEY /* <705>
                                                          MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_PKT_RD /* <706>
                                                          MEMDB_IFTA100_MEM1_0_KEY_DOP_MEM1_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_KEY /* <707>
                                                          MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_PKT_RD /* <708>
                                                          MEMDB_IFTA100_MEM11_1_KEY_DOP_MEM11_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_KEY /* <709>
                                                          MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_PKT_RD /* <710>
                                                          MEMDB_IFTA100_MEM2_2_KEY_DOP_MEM2_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_KEY /* <711>
                                                          MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_PKT_RD /* <712>
                                                          MEMDB_IFTA100_MEM8_1_KEY_DOP_MEM8_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_KEY /* <713>
                                                          MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_PKT_RD /* <714>
                                                          MEMDB_IFTA100_MEM7_1_KEY_DOP_MEM7_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_KEY /* <715>
                                                          MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_PKT_RD /* <716>
                                                          MEMDB_IFTA100_MEM9_3_KEY_DOP_MEM9_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_KEY /* <717>
                                                          MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_PKT_RD /* <718>
                                                          MEMDB_IFTA100_MEM6_0_KEY_DOP_MEM6_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_KEY /* <719>
                                                          MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_PKT_RD /* <720>
                                                          MEMDB_IFTA100_MEM6_2_KEY_DOP_MEM6_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_KEY /* <721>
                                                          MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_PKT_RD /* <722>
                                                          MEMDB_IFTA100_MEM9_1_KEY_DOP_MEM9_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_KEY /* <723>
                                                          MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_PKT_RD /* <724>
                                                          MEMDB_IFTA100_MEM5_1_KEY_DOP_MEM5_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_KEY /* <725>
                                                          MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* <726>
                                                          MEMDB_IFTA100_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_KEY /* <727>
                                                          MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_PKT_RD /* <728>
                                                          MEMDB_IFTA100_MEM3_3_KEY_DOP_MEM3_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_KEY /* <729>
                                                          MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* <730>
                                                          MEMDB_IFTA100_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_KEY /* <731>
                                                          MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_PKT_RD /* <732>
                                                          MEMDB_IFTA100_MEM1_1_KEY_DOP_MEM1_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_KEY /* <733>
                                                          MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_PKT_RD /* <734>
                                                          MEMDB_IFTA100_MEM4_0_KEY_DOP_MEM4_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_KEY /* <735>
                                                          MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_PKT_RD /* <736>
                                                          MEMDB_IFTA100_MEM3_1_KEY_DOP_MEM3_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_KEY /* <737>
                                                          MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_PKT_RD /* <738>
                                                          MEMDB_IFTA100_MEM2_3_KEY_DOP_MEM2_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_KEY /* <739>
                                                          MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_PKT_RD /* <740>
                                                          MEMDB_IFTA100_MEM10_0_KEY_DOP_MEM10_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_KEY /* <741>
                                                          MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_PKT_RD /* <742>
                                                          MEMDB_IFTA100_MEM6_3_KEY_DOP_MEM6_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_KEY /* <743>
                                                          MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* <744>
                                                          MEMDB_IFTA100_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_KEY /* <745>
                                                          MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_PKT_RD /* <746>
                                                          MEMDB_IFTA100_MEM5_2_KEY_DOP_MEM5_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_KEY /* <747>
                                                          MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_PKT_RD /* <748>
                                                          MEMDB_IFTA100_MEM7_0_KEY_DOP_MEM7_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_KEY /* <749>
                                                          MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_PKT_RD /* <750>
                                                          MEMDB_IFTA100_MEM1_3_KEY_DOP_MEM1_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_KEY /* <751>
                                                          MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_PKT_RD /* <752>
                                                          MEMDB_IFTA100_MEM11_2_KEY_DOP_MEM11_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_MATCH_INDEX /* <753>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_TCAM_MATCH /* <754>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_MATCH_INDEX /* <755>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_TCAM_MATCH /* <756>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_MATCH_INDEX /* <757>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_TCAM_MATCH /* <758>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_MATCH_INDEX /* <759>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_TCAM_MATCH /* <760>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP_MEM9_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_MATCH_INDEX /* <761>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_TCAM_MATCH /* <762>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_MATCH_INDEX /* <763>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_TCAM_MATCH /* <764>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_MATCH_INDEX /* <765>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_TCAM_MATCH /* <766>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_MATCH_INDEX /* <767>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_TCAM_MATCH /* <768>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP_MEM10_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX /* <769>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH /* <770>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX /* <771>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH /* <772>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_MATCH_INDEX /* <773>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_TCAM_MATCH /* <774>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_MATCH_INDEX /* <775>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_TCAM_MATCH /* <776>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP_MEM7_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX /* <777>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH /* <778>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX /* <779>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH /* <780>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_MATCH_INDEX /* <781>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_TCAM_MATCH /* <782>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_MATCH_INDEX /* <783>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_TCAM_MATCH /* <784>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP_MEM5_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_MATCH_INDEX /* <785>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_TCAM_MATCH /* <786>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_MATCH_INDEX /* <787>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_TCAM_MATCH /* <788>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_MATCH_INDEX /* <789>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_TCAM_MATCH /* <790>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_MATCH_INDEX /* <791>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_TCAM_MATCH /* <792>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP_MEM8_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX /* <793>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH /* <794>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX /* <795>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH /* <796>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_MATCH_INDEX /* <797>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_TCAM_MATCH /* <798>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_MATCH_INDEX /* <799>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_TCAM_MATCH /* <800>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP_MEM2_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_MATCH_INDEX /* <801>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_TCAM_MATCH /* <802>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_MATCH_INDEX /* <803>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_TCAM_MATCH /* <804>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_MATCH_INDEX /* <805>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_TCAM_MATCH /* <806>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_MATCH_INDEX /* <807>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_TCAM_MATCH /* <808>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP_MEM11_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX /* <809>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH /* <810>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX /* <811>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH /* <812>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_MATCH_INDEX /* <813>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_TCAM_MATCH /* <814>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_MATCH_INDEX /* <815>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_TCAM_MATCH /* <816>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP_MEM6_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX /* <817>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH /* <818>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX /* <819>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH /* <820>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX /* <821>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH /* <822>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX /* <823>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH /* <824>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP_MEM1_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX /* <825>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH /* <826>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX /* <827>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH /* <828>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_MATCH_INDEX /* <829>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_TCAM_MATCH /* <830>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_MATCH_INDEX /* <831>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_TCAM_MATCH /* <832>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP_MEM4_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX /* <833>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH /* <834>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX /* <835>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH /* <836>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_MATCH_INDEX /* <837>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_TCAM_MATCH /* <838>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_MATCH_INDEX /* <839>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_TCAM_MATCH /* <840>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP_MEM3_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* <841>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* <842>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* <843>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* <844>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* <845>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* <846>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* <847>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* <848>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <849>
                                                          IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <850>
                                                          IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <851>
                                                          IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <852>
                                                          IFSL100_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX /* <853>
                                                          ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA /* <854>
                                                          ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX /* <855>
                                                          ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_HASH BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_HASH /* <856>
                                                          ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_HASH. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_MODULO_OFFSET BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_MODULO_OFFSET /* <857>
                                                          ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP_MODULO_OFFSET. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <858>
                                                          MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <859>
                                                          MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <860>
                                                          MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <861>
                                                          MEMDB_IFTA110_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B1 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B1 /* <862>
                                                          ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B1. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B0 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B0 /* <863>
                                                          ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_B0. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A1 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A1 /* <864>
                                                          ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A1. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A0 BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A0 /* <865>
                                                          ETRAP_ETR_HIT_RTAG_HASH_DOP_RTAG7_HASH_A0. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_ETR_HIT BCMPKT_TRACE_DOP_ETRAP_ETR_HIT_RTAG_HASH_DOP_ETR_HIT /* <866>
                                                          ETRAP_ETR_HIT_RTAG_HASH_DOP_ETR_HIT. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_FLW_OFMT_FLOW_TABLE_HIT BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_FLW_OFMT_FLOW_TABLE_HIT /* <867>
                                                          ETRAP_ETR_OUT_DOP_FLW_OFMT_FLOW_TABLE_HIT. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_EOP_FLOW_TABLE_HIT_PM2 BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_EOP_FLOW_TABLE_HIT_PM2 /* <868>
                                                          ETRAP_ETR_OUT_DOP_EOP_FLOW_TABLE_HIT_PM2. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_CNG BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_CNG /* <869>
                                                          ETRAP_ETR_OUT_DOP_ETR_CNG. */
#define BCM_PKTIO_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_VALID BCMPKT_TRACE_DOP_ETRAP_ETR_OUT_DOP_ETR_VALID /* <870>
                                                          ETRAP_ETR_OUT_DOP_ETR_VALID. */
#define BCM_PKTIO_TRACE_DOP_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP_CURRENT_TIME BCMPKT_TRACE_DOP_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP_CURRENT_TIME /* <871>
                                                          DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP_CURRENT_TIME. */
#define BCM_PKTIO_TRACE_DOP_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP_FLOWSET_INDEX BCMPKT_TRACE_DOP_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP_FLOWSET_INDEX /* <872>
                                                          DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP_FLOWSET_INDEX. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX /* <873>
                                                          ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP_SHUFFLE_TABLE_INDEX. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA /* <874>
                                                          ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP_GROUP_TABLE_DATA. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX /* <875>
                                                          ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_ECMP_MEMBER_INDEX. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_HASH BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_HASH /* <876>
                                                          ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_HASH. */
#define BCM_PKTIO_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_MODULO_OFFSET BCMPKT_TRACE_DOP_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_MODULO_OFFSET /* <877>
                                                          ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP_MODULO_OFFSET. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <878>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <879>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <880>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <881>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* <882>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* <883>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* <884>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* <885>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* <886>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* <887>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* <888>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* <889>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* <890>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* <891>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* <892>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* <893>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <894>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <895>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <896>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <897>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* <898>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* <899>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* <900>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* <901>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* <902>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* <903>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* <904>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* <905>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* <906>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* <907>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* <908>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* <909>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA130_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <910>
                                                          IFTA130_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA130_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <911>
                                                          IFTA130_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA130_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <912>
                                                          IFTA130_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_01_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA130_I1T_01_INDEX_DOP_LKP0_LTPR_WIN /* <913>
                                                          IFTA130_I1T_01_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA130_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <914>
                                                          IFTA130_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_02_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA130_I1T_02_INDEX_DOP_LKP0_LTPR_WIN /* <915>
                                                          IFTA130_I1T_02_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA130_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <916>
                                                          IFTA130_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA130_I1T_03_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA130_I1T_03_INDEX_DOP_LKP0_LTPR_WIN /* <917>
                                                          IFTA130_I1T_03_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <918>
                                                          MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <919>
                                                          MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <920>
                                                          MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <921>
                                                          MEMDB_IFTA140_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFTA140_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_IFTA140_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <922>
                                                          IFTA140_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA140_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_IFTA140_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <923>
                                                          IFTA140_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* <924>
                                                          FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* <925>
                                                          FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* <926>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* <927>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* <928>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* <929>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B0 /* <930>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B1 /* <931>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B2 /* <932>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B3 /* <933>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_COUNTER_B3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B0 /* <934>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B1 /* <935>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B2 /* <936>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B3 /* <937>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP_BUS_COUNTER_B3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* <938>
                                                          FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* <939>
                                                          FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* <940>
                                                          FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* <941>
                                                          FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 /* <942>
                                                          FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 /* <943>
                                                          FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 /* <944>
                                                          FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 /* <945>
                                                          FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A0 /* <946>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A1 /* <947>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A2 /* <948>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A3 /* <949>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_BUS_COUNTER_A3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A0 /* <950>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A1 /* <951>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A2 /* <952>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A3 /* <953>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP_COUNTER_A3. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <954>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <955>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <956>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <957>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <958>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <959>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <960>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <961>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <962>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <963>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <964>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <965>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <966>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <967>
                                                          IFTA150_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <968>
                                                          IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <969>
                                                          IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <970>
                                                          IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <971>
                                                          IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <972>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <973>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <974>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <975>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <976>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <977>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <978>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <979>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_KEY /* <980>
                                                          MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* <981>
                                                          MEMDB_IFTA150_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_KEY /* <982>
                                                          MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* <983>
                                                          MEMDB_IFTA150_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_KEY /* <984>
                                                          MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* <985>
                                                          MEMDB_IFTA150_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_KEY /* <986>
                                                          MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* <987>
                                                          MEMDB_IFTA150_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* <988>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* <989>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* <990>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* <991>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* <992>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* <993>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* <994>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* <995>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP_CURRENT_TIME BCMPKT_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP_CURRENT_TIME /* <996>
                                                          IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP_CURRENT_TIME. */
#define BCM_PKTIO_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP_FLOWSET_INDEX BCMPKT_TRACE_DOP_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP_FLOWSET_INDEX /* <997>
                                                          IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP_FLOWSET_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP_MIRROR_BUS BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP_MIRROR_BUS /* <998>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP_MIRROR_BUS. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_0_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_0_RDATA /* <999>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_0_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_1_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_1_RDATA /* <1000>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_1_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_2_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_2_RDATA /* <1001>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_2_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_3_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_3_RDATA /* <1002>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_3_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_4_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_4_RDATA /* <1003>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_4_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_5_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_5_RDATA /* <1004>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_5_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_6_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_6_RDATA /* <1005>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_6_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_7_RDATA BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_7_RDATA /* <1006>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP_FINAL_MIRROR_SESSION_7_RDATA. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_0_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_0_WINNER /* <1007>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_0_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_1_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_1_WINNER /* <1008>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_1_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_2_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_2_WINNER /* <1009>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_2_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_3_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_3_WINNER /* <1010>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_3_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_4_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_4_WINNER /* <1011>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_4_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_5_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_5_WINNER /* <1012>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_5_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_6_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_6_WINNER /* <1013>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_6_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_7_WINNER BCMPKT_TRACE_DOP_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_7_WINNER /* <1014>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP_MIRROR_SESSION_7_WINNER. */
#define BCM_PKTIO_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <1015>
                                                          IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX /* <1016>
                                                          IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH /* <1017>
                                                          IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP_CPU_COS_MAP_TCAM_KEY BCMPKT_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP_CPU_COS_MAP_TCAM_KEY /* <1018>
                                                          IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP_CPU_COS_MAP_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_MATCH_INDEX /* <1019>
                                                          IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_TCAM_MATCH /* <1020>
                                                          IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP_CPU_COS_MAP_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY /* <1021>
                                                          IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_MATCH_INDEX /* <1022>
                                                          IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_TCAM_MATCH /* <1023>
                                                          IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP_ING_MPB_FLEX_DATA_SELECT_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP_OUT_MPB_FLEX_BUS BCMPKT_TRACE_DOP_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP_OUT_MPB_FLEX_BUS /* <1024>
                                                          IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP_OUT_MPB_FLEX_BUS. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP_OUT_CCBI_B_BUS BCMPKT_TRACE_DOP_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP_OUT_CCBI_B_BUS /* <1025>
                                                          IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP_OUT_CCBI_B_BUS. */
#define BCM_PKTIO_TRACE_DOP_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP_OUT_MPB_FIXED_BUS BCMPKT_TRACE_DOP_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP_OUT_MPB_FIXED_BUS /* <1026>
                                                          IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP_OUT_MPB_FIXED_BUS. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP_ING_FLEX_CTR_BUS BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP_ING_FLEX_CTR_BUS /* <1027>
                                                          FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP_ING_FLEX_CTR_BUS. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR /* <1028>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP_COUNTER_EOP_BUFFER_11_PKT_WDATA_COUNTER_EOP_BUFFER_11_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP_COUNTER_EOP_BUFFER_11_PKT_WDATA_COUNTER_EOP_BUFFER_11_PKT_WR /* <1029>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP_COUNTER_EOP_BUFFER_11_PKT_WDATA_COUNTER_EOP_BUFFER_11_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP_COUNTER_EOP_BUFFER_8_PKT_WDATA_COUNTER_EOP_BUFFER_8_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP_COUNTER_EOP_BUFFER_8_PKT_WDATA_COUNTER_EOP_BUFFER_8_PKT_WR /* <1030>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP_COUNTER_EOP_BUFFER_8_PKT_WDATA_COUNTER_EOP_BUFFER_8_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR /* <1031>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR /* <1032>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP_COUNTER_EOP_BUFFER_9_PKT_WDATA_COUNTER_EOP_BUFFER_9_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP_COUNTER_EOP_BUFFER_9_PKT_WDATA_COUNTER_EOP_BUFFER_9_PKT_WR /* <1033>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP_COUNTER_EOP_BUFFER_9_PKT_WDATA_COUNTER_EOP_BUFFER_9_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR /* <1034>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR /* <1035>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR /* <1036>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR /* <1037>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR /* <1038>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP_COUNTER_EOP_BUFFER_10_PKT_WDATA_COUNTER_EOP_BUFFER_10_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP_COUNTER_EOP_BUFFER_10_PKT_WDATA_COUNTER_EOP_BUFFER_10_PKT_WR /* <1039>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP_COUNTER_EOP_BUFFER_10_PKT_WDATA_COUNTER_EOP_BUFFER_10_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <1040>
                                                          IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <1041>
                                                          IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <1042>
                                                          IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <1043>
                                                          IFSL140_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* <1044>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* <1045>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* <1046>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* <1047>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR /* <1048>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP_FIELD_EXTRACTION_PROFILE_CONTROL_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP_FIELD_EXTRACTION_PROFILE_CONTROL_PKT_ADDR /* <1049>
                                                          EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP_FIELD_EXTRACTION_PROFILE_CONTROL_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER0_BUS BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER0_BUS /* <1050>
                                                          EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER0_BUS. */
#define BCM_PKTIO_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER1_BUS BCMPKT_TRACE_DOP_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER1_BUS /* <1051>
                                                          EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP_OUT_EGR_SCR_EPRE2EPARSER1_BUS. */
#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP_EGR_INT_CN_UPDATE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP_EGR_INT_CN_UPDATE_PKT_ADDR /* <1052>
                                                          EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP_EGR_INT_CN_UPDATE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP_FORWARDING_TYPE_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP_FORWARDING_TYPE_TABLE_PKT_ADDR /* <1053>
                                                          EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP_FORWARDING_TYPE_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_MPB_FIXED_DOP_MPB_FIXED BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_MPB_FIXED_DOP_MPB_FIXED /* <1054>
                                                          EPRE_EDEV_CONFIG_MPB_FIXED_DOP_MPB_FIXED. */
#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_CCBE_DOP_CCBE_BUS BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_CCBE_DOP_CCBE_BUS /* <1055>
                                                          EPRE_EDEV_CONFIG_CCBE_DOP_CCBE_BUS. */
#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP_EGR_TABLE_INDEX_UPDATE_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP_EGR_TABLE_INDEX_UPDATE_PROFILE_PKT_ADDR /* <1056>
                                                          EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP_EGR_TABLE_INDEX_UPDATE_PROFILE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP_MIRROR_ATTRIBUTES_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP_MIRROR_ATTRIBUTES_TABLE_PKT_ADDR /* <1057>
                                                          EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP_MIRROR_ATTRIBUTES_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_PDD_PROFILE_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_PDD_PROFILE_TABLE_PKT_ADDR /* <1058>
                                                          EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_PDD_PROFILE_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_MPB_FLEX BCMPKT_TRACE_DOP_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_MPB_FLEX /* <1059>
                                                          EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP_MPB_FLEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <1060>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <1061>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <1062>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <1063>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* <1064>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* <1065>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* <1066>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* <1067>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* <1068>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* <1069>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* <1070>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* <1071>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* <1072>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* <1073>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* <1074>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* <1075>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA10_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1076>
                                                          EFTA10_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <1077>
                                                          EFTA10_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA10_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1078>
                                                          EFTA10_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_01_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA10_I1T_01_INDEX_DOP_LKP0_LTPR_WIN /* <1079>
                                                          EFTA10_I1T_01_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA10_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1080>
                                                          EFTA10_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_02_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA10_I1T_02_INDEX_DOP_LKP0_LTPR_WIN /* <1081>
                                                          EFTA10_I1T_02_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA10_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1082>
                                                          EFTA10_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA10_I1T_03_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA10_I1T_03_INDEX_DOP_LKP0_LTPR_WIN /* <1083>
                                                          EFTA10_I1T_03_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR /* <1084>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD /* <1085>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR /* <1086>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD /* <1087>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_ADDR /* <1088>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_RD /* <1089>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM10_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_ADDR /* <1090>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_RD /* <1091>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM11_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR /* <1092>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_RD /* <1093>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR /* <1094>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_RD /* <1095>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR /* <1096>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_RD /* <1097>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM4_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR /* <1098>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_RD /* <1099>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM5_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR /* <1100>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_RD /* <1101>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM6_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR /* <1102>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_RD /* <1103>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM7_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_ADDR /* <1104>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_RD /* <1105>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM8_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_ADDR BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_ADDR /* <1106>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_RD BCMPKT_TRACE_DOP_MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_RD /* <1107>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP_MEM9_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA20_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1108>
                                                          EFTA20_I1T_00_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN /* <1109>
                                                          EFTA20_I1T_00_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA20_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1110>
                                                          EFTA20_I1T_01_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_01_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA20_I1T_01_INDEX_DOP_LKP0_LTPR_WIN /* <1111>
                                                          EFTA20_I1T_01_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA20_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1112>
                                                          EFTA20_I1T_02_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_02_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA20_I1T_02_INDEX_DOP_LKP0_LTPR_WIN /* <1113>
                                                          EFTA20_I1T_02_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA20_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1114>
                                                          EFTA20_I1T_03_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_03_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA20_I1T_03_INDEX_DOP_LKP0_LTPR_WIN /* <1115>
                                                          EFTA20_I1T_03_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_04_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA20_I1T_04_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1116>
                                                          EFTA20_I1T_04_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_04_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA20_I1T_04_INDEX_DOP_LKP0_LTPR_WIN /* <1117>
                                                          EFTA20_I1T_04_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_05_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA20_I1T_05_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1118>
                                                          EFTA20_I1T_05_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_05_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA20_I1T_05_INDEX_DOP_LKP0_LTPR_WIN /* <1119>
                                                          EFTA20_I1T_05_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_06_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT BCMPKT_TRACE_DOP_EFTA20_I1T_06_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT /* <1120>
                                                          EFTA20_I1T_06_INDEX_DOP_LKP0_TILE_KEY_COMPRESSION_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA20_I1T_06_INDEX_DOP_LKP0_LTPR_WIN BCMPKT_TRACE_DOP_EFTA20_I1T_06_INDEX_DOP_LKP0_LTPR_WIN /* <1121>
                                                          EFTA20_I1T_06_INDEX_DOP_LKP0_LTPR_WIN. */
#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <1122>
                                                          EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <1123>
                                                          EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <1124>
                                                          EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <1125>
                                                          EFSL20_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <1126>
                                                          EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <1127>
                                                          EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <1128>
                                                          EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <1129>
                                                          EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <1130>
                                                          EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <1131>
                                                          EFTA30_E2T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <1132>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <1133>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <1134>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <1135>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <1136>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <1137>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <1138>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_EFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <1139>
                                                          EFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY /* <1140>
                                                          EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP_LTS_TCAM_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY /* <1141>
                                                          EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP_LTS_TCAM_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY /* <1142>
                                                          EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP_LTS_TCAM_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY /* <1143>
                                                          EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP_LTS_TCAM_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX /* <1144>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH /* <1145>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX /* <1146>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH /* <1147>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX /* <1148>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH /* <1149>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX /* <1150>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH /* <1151>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA /* <1152>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <1153>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE /* <1154>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP0_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH /* <1155>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <1156>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE /* <1157>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP1_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH /* <1158>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <1159>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE /* <1160>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP2_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH /* <1161>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID /* <1162>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_LOGICAL_TABLE_ID. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE /* <1163>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LKP3_LTS_TCAM_DATA_KEY_TYPE. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH /* <1164>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP_LTS_TCAM_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFTA30_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT BCMPKT_TRACE_DOP_EFTA30_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT /* <1165>
                                                          EFTA30_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP_LTS_PRE_SEL_MUX_OUT. */
#define BCM_PKTIO_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_TS_PROFILE_INDEX BCMPKT_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_TS_PROFILE_INDEX /* <1166>
                                                          EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_TS_PROFILE_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_INGRESS_TIMESTAMP_BUS BCMPKT_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_INGRESS_TIMESTAMP_BUS /* <1167>
                                                          EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_EGR_INGRESS_TIMESTAMP_BUS. */
#define BCM_PKTIO_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_TIMESTAMP_COUNT BCMPKT_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_TIMESTAMP_COUNT /* <1168>
                                                          EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_TIMESTAMP_COUNT. */
#define BCM_PKTIO_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_INGRESS_TIMESTAMP BCMPKT_TRACE_DOP_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_INGRESS_TIMESTAMP /* <1169>
                                                          EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP_INGRESS_TIMESTAMP. */
#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBER_INDEX BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBER_INDEX /* <1170>
                                                          EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBER_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_FIELD_BITMAP_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_FIELD_BITMAP_PKT_ADDR /* <1171>
                                                          EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_FIELD_BITMAP_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_UNTAG_BITMAP_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_UNTAG_BITMAP_PKT_ADDR /* <1172>
                                                          EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_UNTAG_BITMAP_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_STATE_PROFILE_LOWER_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_STATE_PROFILE_LOWER_PKT_ADDR /* <1173>
                                                          EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_STATE_PROFILE_LOWER_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBERSHIP_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBERSHIP_PROFILE_PKT_ADDR /* <1174>
                                                          EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP_MEMBERSHIP_PROFILE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* <1175>
                                                          FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* <1176>
                                                          FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* <1177>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* <1178>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* <1179>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* <1180>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B0 /* <1181>
                                                          FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B1 /* <1182>
                                                          FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B2 /* <1183>
                                                          FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B3 /* <1184>
                                                          FLEX_CTR_ST_EGR_COUNTER_B_DOP_COUNTER_B3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* <1185>
                                                          FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* <1186>
                                                          FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* <1187>
                                                          FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* <1188>
                                                          FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A0 /* <1189>
                                                          FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A1 /* <1190>
                                                          FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A2 /* <1191>
                                                          FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A3 /* <1192>
                                                          FLEX_CTR_ST_EGR_COUNTER_A_DOP_COUNTER_A3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* <1193>
                                                          FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* <1194>
                                                          FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* <1195>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* <1196>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* <1197>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* <1198>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B0 /* <1199>
                                                          FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B1 /* <1200>
                                                          FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B2 /* <1201>
                                                          FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B3 /* <1202>
                                                          FLEX_CTR_ST_EGR0_COUNTER_B_DOP_BUS_COUNTER_B3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* <1203>
                                                          FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* <1204>
                                                          FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* <1205>
                                                          FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* <1206>
                                                          FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A0 /* <1207>
                                                          FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A1 /* <1208>
                                                          FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A2 /* <1209>
                                                          FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A3 /* <1210>
                                                          FLEX_CTR_ST_EGR0_COUNTER_A_DOP_BUS_COUNTER_A3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 /* <1211>
                                                          FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 /* <1212>
                                                          FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 /* <1213>
                                                          FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 /* <1214>
                                                          FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0 /* <1215>
                                                          FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1 /* <1216>
                                                          FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP_FLEX_STATE_ACTION_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID /* <1217>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_1_UPDATE_CMD_DOP_POOL1_COUNTER_UPDATE_CMDPOOL1_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID /* <1218>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_2_UPDATE_CMD_DOP_POOL2_COUNTER_UPDATE_CMDPOOL2_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID /* <1219>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_3_UPDATE_CMD_DOP_POOL3_COUNTER_UPDATE_CMDPOOL3_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID /* <1220>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_0_UPDATE_CMD_DOP_POOL0_COUNTER_UPDATE_CMDPOOL0_CMD_VALID. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B0 /* <1221>
                                                          FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B1 /* <1222>
                                                          FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B2 /* <1223>
                                                          FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B3 /* <1224>
                                                          FLEX_CTR_ST_EGR1_COUNTER_B_DOP_BUS_COUNTER_B3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0 /* <1225>
                                                          FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1 /* <1226>
                                                          FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2 /* <1227>
                                                          FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3 /* <1228>
                                                          FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP_TRIGGER_STATE_CTRL_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A0 /* <1229>
                                                          FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A1 /* <1230>
                                                          FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A2 /* <1231>
                                                          FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A3 /* <1232>
                                                          FLEX_CTR_ST_EGR1_COUNTER_A_DOP_BUS_COUNTER_A3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0 /* <1233>
                                                          FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1 /* <1234>
                                                          FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2 /* <1235>
                                                          FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 BCMPKT_TRACE_DOP_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3 /* <1236>
                                                          FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP_TRUTH_TABLE_OUTPUT_3. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_KEY /* <1237>
                                                          MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_PKT_RD /* <1238>
                                                          MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP_MEM0_1_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_KEY /* <1239>
                                                          MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_PKT_RD /* <1240>
                                                          MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP_MEM0_3_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_KEY /* <1241>
                                                          MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_PKT_RD /* <1242>
                                                          MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP_MEM0_2_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_KEY BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_KEY /* <1243>
                                                          MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_KEY. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_PKT_RD BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_PKT_RD /* <1244>
                                                          MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP_MEM0_0_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX /* <1245>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH /* <1246>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_0_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX /* <1247>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH /* <1248>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_1_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX /* <1249>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH /* <1250>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_2_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX /* <1251>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH BCMPKT_TRACE_DOP_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH /* <1252>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP_MEM0_3_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <1253>
                                                          EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD /* <1254>
                                                          EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_PKT_RD. */
#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX /* <1255>
                                                          EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH /* <1256>
                                                          EFSL30_LTS_TCAM_KEY_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP_OUT_EGR_SCR_MIRROR2EDIT_CTRL_BUS BCMPKT_TRACE_DOP_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP_OUT_EGR_SCR_MIRROR2EDIT_CTRL_BUS /* <1257>
                                                          EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP_OUT_EGR_SCR_MIRROR2EDIT_CTRL_BUS. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP_LTS_TCAM_ONLY_KEY BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP_LTS_TCAM_ONLY_KEY /* <1258>
                                                          QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP_LTS_TCAM_ONLY_KEY. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY /* <1259>
                                                          QOS_REMARKING_LTS_TCAM_KEY_DOP_LTS_TCAM_KEY. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_MATCH_INDEX BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_MATCH_INDEX /* <1260>
                                                          QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH /* <1261>
                                                          QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_POLICY_DATA BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_POLICY_DATA /* <1262>
                                                          QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_POLICY_DATA. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX /* <1263>
                                                          QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH /* <1264>
                                                          QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_ONLY_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA /* <1265>
                                                          QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX /* <1266>
                                                          QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_POLICY_DATA_HIT_INDEX. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH BCMPKT_TRACE_DOP_QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH /* <1267>
                                                          QOS_REMARKING_LTS_TCAM_INDEX_DOP_LTS_TCAM_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_4_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_4_PKT_ADDR /* <1268>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_4_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_3_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_3_PKT_ADDR /* <1269>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_2_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_2_PKT_ADDR /* <1270>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_1_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_1_PKT_ADDR /* <1271>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_0_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_0_PKT_ADDR /* <1272>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_ECN_MAP_TABLE_0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_1_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_1_PKT_ADDR /* <1273>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_0_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_0_PKT_ADDR /* <1274>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_MPLS_QOS_MAP_TABLE_0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_4_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_4_PKT_ADDR /* <1275>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_4_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_3_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_3_PKT_ADDR /* <1276>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_3_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_2_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_2_PKT_ADDR /* <1277>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_2_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_1_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_1_PKT_ADDR /* <1278>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_1_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_0_PKT_ADDR BCMPKT_TRACE_DOP_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_0_PKT_ADDR /* <1279>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP_QOS_MAP_TABLE_0_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP_TCAM_4_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP_TCAM_4_A_KEY /* <1280>
                                                          EDIT_CTRL_ZONE_4_TCAM_KEY_DOP_TCAM_4_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP_TCAM_3_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP_TCAM_3_A_KEY /* <1281>
                                                          EDIT_CTRL_ZONE_3_TCAM_KEY_DOP_TCAM_3_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP_TCAM_1_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP_TCAM_1_A_KEY /* <1282>
                                                          EDIT_CTRL_ZONE_1_TCAM_KEY_DOP_TCAM_1_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP_TCAM_2_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP_TCAM_2_A_KEY /* <1283>
                                                          EDIT_CTRL_ZONE_2_TCAM_KEY_DOP_TCAM_2_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP_TCAM_0_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP_TCAM_0_A_KEY /* <1284>
                                                          EDIT_CTRL_ZONE_0_TCAM_KEY_DOP_TCAM_0_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_KEY_DOP_TCAM_4_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_KEY_DOP_TCAM_4_A_KEY /* <1285>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_KEY_DOP_TCAM_4_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_KEY_DOP_TCAM_3_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_KEY_DOP_TCAM_3_A_KEY /* <1286>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_KEY_DOP_TCAM_3_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_KEY_DOP_TCAM_2_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_KEY_DOP_TCAM_2_A_KEY /* <1287>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_KEY_DOP_TCAM_2_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_KEY_DOP_TCAM_1_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_KEY_DOP_TCAM_1_A_KEY /* <1288>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_KEY_DOP_TCAM_1_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_KEY_DOP_TCAM_0_A_KEY BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_KEY_DOP_TCAM_0_A_KEY /* <1289>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_KEY_DOP_TCAM_0_A_KEY. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_TCAM_MATCH /* <1290>
                                                          EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_MATCH_INDEX /* <1291>
                                                          EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_TCAM_MATCH /* <1292>
                                                          EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_MATCH_INDEX /* <1293>
                                                          EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_TCAM_MATCH /* <1294>
                                                          EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_MATCH_INDEX /* <1295>
                                                          EDIT_CTRL_ZONE_0_TCAM_HIT_DOP_TCAM_0_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_TCAM_MATCH /* <1296>
                                                          EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_MATCH_INDEX /* <1297>
                                                          EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_TCAM_MATCH /* <1298>
                                                          EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_MATCH_INDEX /* <1299>
                                                          EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_TCAM_MATCH /* <1300>
                                                          EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_MATCH_INDEX /* <1301>
                                                          EDIT_CTRL_ZONE_4_TCAM_HIT_DOP_TCAM_4_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_TCAM_MATCH /* <1302>
                                                          EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_MATCH_INDEX /* <1303>
                                                          EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_TCAM_MATCH /* <1304>
                                                          EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_MATCH_INDEX /* <1305>
                                                          EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_TCAM_MATCH /* <1306>
                                                          EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_MATCH_INDEX /* <1307>
                                                          EDIT_CTRL_ZONE_3_TCAM_HIT_DOP_TCAM_3_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_TCAM_MATCH /* <1308>
                                                          EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_MATCH_INDEX /* <1309>
                                                          EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_TCAM_MATCH /* <1310>
                                                          EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_MATCH_INDEX /* <1311>
                                                          EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_TCAM_MATCH /* <1312>
                                                          EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_MATCH_INDEX /* <1313>
                                                          EDIT_CTRL_ZONE_2_TCAM_HIT_DOP_TCAM_2_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_TCAM_MATCH /* <1314>
                                                          EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_MATCH_INDEX /* <1315>
                                                          EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_TCAM_MATCH /* <1316>
                                                          EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_MATCH_INDEX /* <1317>
                                                          EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_TCAM_MATCH /* <1318>
                                                          EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_MATCH_INDEX /* <1319>
                                                          EDIT_CTRL_ZONE_1_TCAM_HIT_DOP_TCAM_1_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_C_TCAM_MATCH /* <1320>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_C_MATCH_INDEX /* <1321>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_B_TCAM_MATCH /* <1322>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_B_MATCH_INDEX /* <1323>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_A_TCAM_MATCH /* <1324>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_A_MATCH_INDEX /* <1325>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP_TCAM_0_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_C_TCAM_MATCH /* <1326>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_C_MATCH_INDEX /* <1327>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_B_TCAM_MATCH /* <1328>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_B_MATCH_INDEX /* <1329>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_A_TCAM_MATCH /* <1330>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_A_MATCH_INDEX /* <1331>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP_TCAM_1_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_C_TCAM_MATCH /* <1332>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_C_MATCH_INDEX /* <1333>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_B_TCAM_MATCH /* <1334>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_B_MATCH_INDEX /* <1335>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_A_TCAM_MATCH /* <1336>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_A_MATCH_INDEX /* <1337>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP_TCAM_2_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_C_TCAM_MATCH /* <1338>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_C_MATCH_INDEX /* <1339>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_B_TCAM_MATCH /* <1340>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_B_MATCH_INDEX /* <1341>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_A_TCAM_MATCH /* <1342>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_A_MATCH_INDEX /* <1343>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP_TCAM_3_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_C_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_C_TCAM_MATCH /* <1344>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_C_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_C_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_C_MATCH_INDEX /* <1345>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_C_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_B_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_B_TCAM_MATCH /* <1346>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_B_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_B_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_B_MATCH_INDEX /* <1347>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_B_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_A_TCAM_MATCH BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_A_TCAM_MATCH /* <1348>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_A_TCAM_MATCH. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_A_MATCH_INDEX BCMPKT_TRACE_DOP_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_A_MATCH_INDEX /* <1349>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP_TCAM_4_A_MATCH_INDEX. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1350>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1351>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1352>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1353>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1354>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1355>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1356>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1357>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1358>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1359>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1360>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1361>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS BCMPKT_TRACE_DOP_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS /* <1362>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP_OUT_EGR_SCR_EDIT_CTRL2EDITOR_BUS. */
#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP_MIRROR_SEQUENCE_NUMBER_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP_MIRROR_SEQUENCE_NUMBER_PROFILE_PKT_ADDR /* <1363>
                                                          EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP_MIRROR_SEQUENCE_NUMBER_PROFILE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP_NUMBER_TABLE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP_NUMBER_TABLE_PKT_ADDR /* <1364>
                                                          EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP_NUMBER_TABLE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP_PROFILE_PKT_ADDR BCMPKT_TRACE_DOP_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP_PROFILE_PKT_ADDR /* <1365>
                                                          EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP_PROFILE_PKT_ADDR. */
#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP_NEW_SEQUENCE_NUM BCMPKT_TRACE_DOP_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP_NEW_SEQUENCE_NUM /* <1366>
                                                          EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP_NEW_SEQUENCE_NUM. */
#define BCM_PKTIO_TRACE_DOP_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP_PKT_SEQUENCE_NUM BCMPKT_TRACE_DOP_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP_PKT_SEQUENCE_NUM /* <1367>
                                                          EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP_PKT_SEQUENCE_NUM. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_4 /* <1368>
                                                          FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_4. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_3 /* <1369>
                                                          FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_2 /* <1370>
                                                          FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_1 /* <1371>
                                                          FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_0 /* <1372>
                                                          FLEX_EDITOR_MATCH_ID_DOP_MATCH_ID_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_3 /* <1373>
                                                          FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_2 /* <1374>
                                                          FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_1 /* <1375>
                                                          FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_0 /* <1376>
                                                          FLEX_EDITOR_VHLEN_DOP_EGR_VHLEN_FMT_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_4 /* <1377>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_4. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_3 /* <1378>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_2 /* <1379>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_1 /* <1380>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_0 /* <1381>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_RW_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_4 /* <1382>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_4. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_3 /* <1383>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_2 /* <1384>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_1 /* <1385>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_0 /* <1386>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_INS_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_4 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_4 /* <1387>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_4. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_3 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_3 /* <1388>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_3. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_2 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_2 /* <1389>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_2. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_1 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_1 /* <1390>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_1. */
#define BCM_PKTIO_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_0 BCMPKT_TRACE_DOP_FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_0 /* <1391>
                                                          FLEX_EDITOR_EDIT_ID_DOP_EDIT_ID_DEL_0. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP_EGR_FLEX_CTR_BUS BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP_EGR_FLEX_CTR_BUS /* <1392>
                                                          FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP_EGR_FLEX_CTR_BUS. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR /* <1393>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP_COUNTER_EOP_BUFFER_2_PKT_WDATA_COUNTER_EOP_BUFFER_2_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR /* <1394>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP_COUNTER_EOP_BUFFER_7_PKT_WDATA_COUNTER_EOP_BUFFER_7_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR /* <1395>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP_COUNTER_EOP_BUFFER_5_PKT_WDATA_COUNTER_EOP_BUFFER_5_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR /* <1396>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP_COUNTER_EOP_BUFFER_6_PKT_WDATA_COUNTER_EOP_BUFFER_6_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR /* <1397>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP_COUNTER_EOP_BUFFER_0_PKT_WDATA_COUNTER_EOP_BUFFER_0_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR /* <1398>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP_COUNTER_EOP_BUFFER_1_PKT_WDATA_COUNTER_EOP_BUFFER_1_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR /* <1399>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP_COUNTER_EOP_BUFFER_4_PKT_WDATA_COUNTER_EOP_BUFFER_4_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR BCMPKT_TRACE_DOP_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR /* <1400>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP_COUNTER_EOP_BUFFER_3_PKT_WDATA_COUNTER_EOP_BUFFER_3_PKT_WR. */
#define BCM_PKTIO_TRACE_DOP_FID_COUNT       BCMPKT_TRACE_DOP_FID_COUNT /* <1401> TRACE_DOP FIELD
                                                          ID NUMBER */

#define BCM_PKTIO_TRACE_DOP_ID_FID_INVALID  BCMPKT_TRACE_DOP_ID_FID_INVALID /* <-1> Invalid
                                                          BCM_PKTIO_TRACE_DOP_ID
                                                          FID indicator */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER0_HME_STAGE0_DOP BCMPKT_TRACE_DOP_ID_IPARSER0_HME_STAGE0_DOP /* <0>
                                                          IPARSER0_HME_STAGE0_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA10_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA10_MY_DOP_INDEX_DOP /* <1>
                                                          MEMDB_IFTA10_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA10_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA10_I1T_00_INDEX_DOP /* <2>
                                                          IFTA10_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA20_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA20_MY_DOP_INDEX_DOP /* <3>
                                                          MEMDB_IFTA20_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA20_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA20_I1T_00_INDEX_DOP /* <4>
                                                          IFTA20_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE0_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE0_DOP /* <5>
                                                          IPARSER1_HME_STAGE0_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE1_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE1_DOP /* <6>
                                                          IPARSER1_HME_STAGE1_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE2_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE2_DOP /* <7>
                                                          IPARSER1_HME_STAGE2_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE3_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE3_DOP /* <8>
                                                          IPARSER1_HME_STAGE3_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE4_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE4_DOP /* <9>
                                                          IPARSER1_HME_STAGE4_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER1_HME_STAGE5_DOP BCMPKT_TRACE_DOP_ID_IPARSER1_HME_STAGE5_DOP /* <10>
                                                          IPARSER1_HME_STAGE5_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP /* <11>
                                                          IFTA30_E2T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP /* <12>
                                                          IFTA30_E2T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_TCAM_INDEX_DOP /* <13>
                                                          IFTA30_E2T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP /* <14>
                                                          IFTA30_E2T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <15>
                                                          IFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE0_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE0_DOP /* <16>
                                                          IPARSER2_HME_STAGE0_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE1_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE1_DOP /* <17>
                                                          IPARSER2_HME_STAGE1_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE2_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE2_DOP /* <18>
                                                          IPARSER2_HME_STAGE2_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE3_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE3_DOP /* <19>
                                                          IPARSER2_HME_STAGE3_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPARSER2_HME_STAGE4_DOP BCMPKT_TRACE_DOP_ID_IPARSER2_HME_STAGE4_DOP /* <20>
                                                          IPARSER2_HME_STAGE4_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP /* <21>
                                                          IFTA40_E2T_01_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP /* <22>
                                                          IFTA40_E2T_01_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_TCAM_INDEX_DOP /* <23>
                                                          IFTA40_E2T_01_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP /* <24>
                                                          IFTA40_E2T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP /* <25>
                                                          IFTA40_E2T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_TCAM_INDEX_DOP /* <26>
                                                          IFTA40_E2T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP /* <27>
                                                          IFTA40_E2T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <28>
                                                          IFTA40_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP /* <29>
                                                          IFTA40_E2T_01_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP /* <30>
                                                          IFTA40_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP /* <31>
                                                          IFTA40_E2T_02_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP /* <32>
                                                          IFTA40_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP /* <33>
                                                          IFTA40_T4T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP /* <34>
                                                          IFTA40_T4T_00_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP /* <35>
                                                          IFTA40_T4T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP /* <36>
                                                          IFTA40_T4T_00_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_TCAM_INDEX_DOP /* <37>
                                                          IFTA40_T4T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP /* <38>
                                                          IFTA40_T4T_01_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP /* <39>
                                                          IFTA40_T4T_01_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP /* <40>
                                                          IFTA40_T4T_01_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP /* <41>
                                                          IFTA40_T4T_01_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_TCAM_INDEX_DOP /* <42>
                                                          IFTA40_T4T_01_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP /* <43>
                                                          IFTA40_T4T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <44>
                                                          IFTA40_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP /* <45>
                                                          IFTA40_T4T_01_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA40_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA40_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP /* <46>
                                                          IFTA40_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP /* <47>
                                                          MEMDB_TCAM_IFTA40_MEM0_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP /* <48>
                                                          MEMDB_TCAM_IFTA40_MEM1_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP /* <49>
                                                          MEMDB_TCAM_IFTA40_MEM0_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP /* <50>
                                                          MEMDB_TCAM_IFTA40_MEM0_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP /* <51>
                                                          MEMDB_TCAM_IFTA40_MEM1_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP /* <52>
                                                          MEMDB_TCAM_IFTA40_MEM1_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP /* <53>
                                                          MEMDB_TCAM_IFTA40_MEM0_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP /* <54>
                                                          MEMDB_TCAM_IFTA40_MEM1_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP /* <55>
                                                          MEMDB_TCAM_IFTA40_MEM0_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP /* <56>
                                                          MEMDB_TCAM_IFTA40_MEM1_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFSL40_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL40_LTS_TCAM_KEY_DOP /* <57>
                                                          IFSL40_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFSL41_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL41_LTS_TCAM_KEY_DOP /* <58>
                                                          IFSL41_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP /* <59>
                                                          IFTA50_T4T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP /* <60>
                                                          IFTA50_T4T_00_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP /* <61>
                                                          IFTA50_T4T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP /* <62>
                                                          IFTA50_T4T_00_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA50_T4T_00_LTS_TCAM_INDEX_DOP /* <63>
                                                          IFTA50_T4T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA50_I1T_00_INDEX_DOP /* <64>
                                                          IFTA50_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA50_I1T_01_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA50_I1T_01_INDEX_DOP /* <65>
                                                          IFTA50_I1T_01_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP /* <66>
                                                          MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP BCMPKT_TRACE_DOP_ID_MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP /* <67>
                                                          MPLS_CTRL_PKT_PROC_MPLS_CTRL_TCAM_HIT_AND_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP /* <68>
                                                          MEMDB_TCAM_IFTA50_MEM0_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP /* <69>
                                                          MEMDB_TCAM_IFTA50_MEM0_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP /* <70>
                                                          MEMDB_TCAM_IFTA50_MEM0_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP /* <71>
                                                          MEMDB_TCAM_IFTA50_MEM0_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP /* <72>
                                                          MEMDB_TCAM_IFTA50_MEM0_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA50_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA50_MY_DOP_INDEX_DOP /* <73>
                                                          MEMDB_IFTA50_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA60_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA60_MY_DOP_INDEX_DOP /* <74>
                                                          MEMDB_IFTA60_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA60_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA60_I1T_00_INDEX_DOP /* <75>
                                                          IFTA60_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA60_I1T_01_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA60_I1T_01_INDEX_DOP /* <76>
                                                          IFTA60_I1T_01_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA60_I1T_02_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA60_I1T_02_INDEX_DOP /* <77>
                                                          IFTA60_I1T_02_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP /* <78>
                                                          IFTA60_T4T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA60_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA60_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <79>
                                                          IFTA60_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP /* <80>
                                                          MEMDB_TCAM_IFTA60_MEM0_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP /* <81>
                                                          MEMDB_TCAM_IFTA60_MEM0_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP /* <82>
                                                          MEMDB_TCAM_IFTA60_MEM0_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP /* <83>
                                                          MEMDB_TCAM_IFTA60_MEM0_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP /* <84>
                                                          MEMDB_TCAM_IFTA60_MEM0_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP /* <85>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP /* <86>
                                                          FLEX_DIGEST_LKUP_FD_NET_LAYER_HIT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_NORM_FD_NORM_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_NORM_FD_NORM_DOP /* <87>
                                                          FLEX_DIGEST_NORM_FD_NORM_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_DIGEST_HASH_FD_HASH_DOP BCMPKT_TRACE_DOP_ID_FLEX_DIGEST_HASH_FD_HASH_DOP /* <88>
                                                          FLEX_DIGEST_HASH_FD_HASH_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA70_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA70_MY_DOP_INDEX_DOP /* <89>
                                                          MEMDB_IFTA70_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA70_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA70_I1T_00_INDEX_DOP /* <90>
                                                          IFTA70_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA70_I1T_01_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA70_I1T_01_INDEX_DOP /* <91>
                                                          IFTA70_I1T_01_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFSL70_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL70_LTS_TCAM_KEY_DOP /* <92>
                                                          IFSL70_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP /* <93>
                                                          IFTA80_E2T_01_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP /* <94>
                                                          IFTA80_E2T_01_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_TCAM_INDEX_DOP /* <95>
                                                          IFTA80_E2T_01_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP /* <96>
                                                          IFTA80_E2T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP /* <97>
                                                          IFTA80_E2T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_TCAM_INDEX_DOP /* <98>
                                                          IFTA80_E2T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP /* <99>
                                                          IFTA80_E2T_03_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP /* <100>
                                                          IFTA80_E2T_03_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_TCAM_INDEX_DOP /* <101>
                                                          IFTA80_E2T_03_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP /* <102>
                                                          IFTA80_E2T_02_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP /* <103>
                                                          IFTA80_E2T_02_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_TCAM_INDEX_DOP /* <104>
                                                          IFTA80_E2T_02_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP /* <105>
                                                          IFTA80_E2T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <106>
                                                          IFTA80_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP /* <107>
                                                          IFTA80_E2T_01_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP /* <108>
                                                          IFTA80_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP /* <109>
                                                          IFTA80_E2T_02_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP /* <110>
                                                          IFTA80_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP /* <111>
                                                          IFTA80_E2T_03_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP /* <112>
                                                          IFTA80_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP /* <113>
                                                          IFTA80_T2T_01_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP /* <114>
                                                          IFTA80_T2T_01_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_TCAM_INDEX_DOP /* <115>
                                                          IFTA80_T2T_01_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP /* <116>
                                                          IFTA80_T2T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP /* <117>
                                                          IFTA80_T2T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_TCAM_INDEX_DOP /* <118>
                                                          IFTA80_T2T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP /* <119>
                                                          IFTA80_T2T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <120>
                                                          IFTA80_T2T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP /* <121>
                                                          IFTA80_T2T_01_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA80_T2T_01_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA80_T2T_01_LTS_PRE_SEL_MUX_OUT_DOP /* <122>
                                                          IFTA80_T2T_01_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_KEY_DOP /* <123>
                                                          FLEX_QOS_PHB_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP /* <124>
                                                          FLEX_QOS_PHB_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_QOS_PHB2_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_FLEX_QOS_PHB2_LTS_TCAM_KEY_DOP /* <125>
                                                          FLEX_QOS_PHB2_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP /* <126>
                                                          FLEX_QOS_PHB2_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP /* <127>
                                                          MEMDB_TCAM_IFTA80_MEM2_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP /* <128>
                                                          MEMDB_TCAM_IFTA80_MEM1_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP /* <129>
                                                          MEMDB_TCAM_IFTA80_MEM7_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP /* <130>
                                                          MEMDB_TCAM_IFTA80_MEM6_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP /* <131>
                                                          MEMDB_TCAM_IFTA80_MEM5_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP /* <132>
                                                          MEMDB_TCAM_IFTA80_MEM2_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP /* <133>
                                                          MEMDB_TCAM_IFTA80_MEM5_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP /* <134>
                                                          MEMDB_TCAM_IFTA80_MEM0_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP /* <135>
                                                          MEMDB_TCAM_IFTA80_MEM6_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP /* <136>
                                                          MEMDB_TCAM_IFTA80_MEM1_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP /* <137>
                                                          MEMDB_TCAM_IFTA80_MEM4_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP /* <138>
                                                          MEMDB_TCAM_IFTA80_MEM3_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP /* <139>
                                                          MEMDB_TCAM_IFTA80_MEM0_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP /* <140>
                                                          MEMDB_TCAM_IFTA80_MEM4_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP /* <141>
                                                          MEMDB_TCAM_IFTA80_MEM7_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP /* <142>
                                                          MEMDB_TCAM_IFTA80_MEM3_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP /* <143>
                                                          MEMDB_TCAM_IFTA80_MEM6_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP /* <144>
                                                          MEMDB_TCAM_IFTA80_MEM1_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP /* <145>
                                                          MEMDB_TCAM_IFTA80_MEM4_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP /* <146>
                                                          MEMDB_TCAM_IFTA80_MEM3_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP /* <147>
                                                          MEMDB_TCAM_IFTA80_MEM7_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP /* <148>
                                                          MEMDB_TCAM_IFTA80_MEM5_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP /* <149>
                                                          MEMDB_TCAM_IFTA80_MEM0_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP /* <150>
                                                          MEMDB_TCAM_IFTA80_MEM2_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP /* <151>
                                                          IFTA90_E2T_03_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP /* <152>
                                                          IFTA90_E2T_03_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_TCAM_INDEX_DOP /* <153>
                                                          IFTA90_E2T_03_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP /* <154>
                                                          IFTA90_E2T_01_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP /* <155>
                                                          IFTA90_E2T_01_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_TCAM_INDEX_DOP /* <156>
                                                          IFTA90_E2T_01_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP /* <157>
                                                          IFTA90_E2T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP /* <158>
                                                          IFTA90_E2T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_TCAM_INDEX_DOP /* <159>
                                                          IFTA90_E2T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP /* <160>
                                                          IFTA90_E2T_02_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP /* <161>
                                                          IFTA90_E2T_02_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_TCAM_INDEX_DOP /* <162>
                                                          IFTA90_E2T_02_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP /* <163>
                                                          IFTA90_E2T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <164>
                                                          IFTA90_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP /* <165>
                                                          IFTA90_E2T_01_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP /* <166>
                                                          IFTA90_E2T_01_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP /* <167>
                                                          IFTA90_E2T_02_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP /* <168>
                                                          IFTA90_E2T_02_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP /* <169>
                                                          IFTA90_E2T_03_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA90_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA90_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP /* <170>
                                                          IFTA90_E2T_03_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP /* <171>
                                                          FLEX_CTR_ST_ING0_COUNTER_ACTION_VECTOR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP /* <172>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_1_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP /* <173>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_2_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP /* <174>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_3_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP /* <175>
                                                          FLEX_CTR_ST_ING0_COUNTER_POOL_0_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_B_DOP /* <176>
                                                          FLEX_CTR_ST_ING0_COUNTER_B_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP /* <177>
                                                          FLEX_CTR_ST_ING0_TRIGGER_STATE_CTRL_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP /* <178>
                                                          FLEX_CTR_ST_ING0_TRUTH_TABLE_CMD_OUTPUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING0_COUNTER_A_DOP /* <179>
                                                          FLEX_CTR_ST_ING0_COUNTER_A_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFSL90_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL90_LTS_TCAM_KEY_DOP /* <180>
                                                          IFSL90_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFSL91_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL91_LTS_TCAM_KEY_DOP /* <181>
                                                          IFSL91_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP /* <182>
                                                          IFTA100_T4T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP /* <183>
                                                          IFTA100_T4T_00_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP /* <184>
                                                          IFTA100_T4T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP /* <185>
                                                          IFTA100_T4T_00_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_TCAM_INDEX_DOP /* <186>
                                                          IFTA100_T4T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP /* <187>
                                                          IFTA100_T4T_02_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP /* <188>
                                                          IFTA100_T4T_02_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP /* <189>
                                                          IFTA100_T4T_02_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP /* <190>
                                                          IFTA100_T4T_02_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_TCAM_INDEX_DOP /* <191>
                                                          IFTA100_T4T_02_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP /* <192>
                                                          IFTA100_T4T_01_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP /* <193>
                                                          IFTA100_T4T_01_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP /* <194>
                                                          IFTA100_T4T_01_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP /* <195>
                                                          IFTA100_T4T_01_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_TCAM_INDEX_DOP /* <196>
                                                          IFTA100_T4T_01_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP /* <197>
                                                          IFTA100_T4T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <198>
                                                          IFTA100_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP /* <199>
                                                          IFTA100_T4T_01_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP /* <200>
                                                          IFTA100_T4T_01_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP /* <201>
                                                          IFTA100_T4T_02_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA100_T4T_02_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA100_T4T_02_LTS_PRE_SEL_MUX_OUT_DOP /* <202>
                                                          IFTA100_T4T_02_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_2_KEY_DOP /* <203>
                                                          MEMDB_IFTA100_MEM1_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_2_KEY_DOP /* <204>
                                                          MEMDB_IFTA100_MEM0_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_2_KEY_DOP /* <205>
                                                          MEMDB_IFTA100_MEM7_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_2_KEY_DOP /* <206>
                                                          MEMDB_IFTA100_MEM8_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_0_KEY_DOP /* <207>
                                                          MEMDB_IFTA100_MEM9_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_3_KEY_DOP /* <208>
                                                          MEMDB_IFTA100_MEM11_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_3_KEY_DOP /* <209>
                                                          MEMDB_IFTA100_MEM7_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_0_KEY_DOP /* <210>
                                                          MEMDB_IFTA100_MEM8_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_2_KEY_DOP /* <211>
                                                          MEMDB_IFTA100_MEM10_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_0_KEY_DOP /* <212>
                                                          MEMDB_IFTA100_MEM5_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_2_KEY_DOP /* <213>
                                                          MEMDB_IFTA100_MEM3_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_3_KEY_DOP /* <214>
                                                          MEMDB_IFTA100_MEM8_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_0_KEY_DOP /* <215>
                                                          MEMDB_IFTA100_MEM11_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_1_KEY_DOP /* <216>
                                                          MEMDB_IFTA100_MEM2_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_3_KEY_DOP /* <217>
                                                          MEMDB_IFTA100_MEM4_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_2_KEY_DOP /* <218>
                                                          MEMDB_IFTA100_MEM4_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_3_KEY_DOP /* <219>
                                                          MEMDB_IFTA100_MEM5_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_1_KEY_DOP /* <220>
                                                          MEMDB_IFTA100_MEM10_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_1_KEY_DOP /* <221>
                                                          MEMDB_IFTA100_MEM6_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_3_KEY_DOP /* <222>
                                                          MEMDB_IFTA100_MEM10_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_1_KEY_DOP /* <223>
                                                          MEMDB_IFTA100_MEM4_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_2_KEY_DOP /* <224>
                                                          MEMDB_IFTA100_MEM9_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_0_KEY_DOP /* <225>
                                                          MEMDB_IFTA100_MEM3_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_0_KEY_DOP /* <226>
                                                          MEMDB_IFTA100_MEM2_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_0_KEY_DOP /* <227>
                                                          MEMDB_IFTA100_MEM1_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_1_KEY_DOP /* <228>
                                                          MEMDB_IFTA100_MEM11_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_2_KEY_DOP /* <229>
                                                          MEMDB_IFTA100_MEM2_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_1_KEY_DOP /* <230>
                                                          MEMDB_IFTA100_MEM8_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_1_KEY_DOP /* <231>
                                                          MEMDB_IFTA100_MEM7_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_3_KEY_DOP /* <232>
                                                          MEMDB_IFTA100_MEM9_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_0_KEY_DOP /* <233>
                                                          MEMDB_IFTA100_MEM6_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_2_KEY_DOP /* <234>
                                                          MEMDB_IFTA100_MEM6_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_1_KEY_DOP /* <235>
                                                          MEMDB_IFTA100_MEM9_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_1_KEY_DOP /* <236>
                                                          MEMDB_IFTA100_MEM5_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_3_KEY_DOP /* <237>
                                                          MEMDB_IFTA100_MEM0_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_3_KEY_DOP /* <238>
                                                          MEMDB_IFTA100_MEM3_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_0_KEY_DOP /* <239>
                                                          MEMDB_IFTA100_MEM0_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_1_KEY_DOP /* <240>
                                                          MEMDB_IFTA100_MEM1_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_0_KEY_DOP /* <241>
                                                          MEMDB_IFTA100_MEM4_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_1_KEY_DOP /* <242>
                                                          MEMDB_IFTA100_MEM3_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_3_KEY_DOP /* <243>
                                                          MEMDB_IFTA100_MEM2_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_0_KEY_DOP /* <244>
                                                          MEMDB_IFTA100_MEM10_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_3_KEY_DOP /* <245>
                                                          MEMDB_IFTA100_MEM6_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_1_KEY_DOP /* <246>
                                                          MEMDB_IFTA100_MEM0_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_2_KEY_DOP /* <247>
                                                          MEMDB_IFTA100_MEM5_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_0_KEY_DOP /* <248>
                                                          MEMDB_IFTA100_MEM7_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_3_KEY_DOP /* <249>
                                                          MEMDB_IFTA100_MEM1_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_2_KEY_DOP /* <250>
                                                          MEMDB_IFTA100_MEM11_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM9_INDEX_DOP /* <251>
                                                          MEMDB_IFTA100_MEM9_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM10_INDEX_DOP /* <252>
                                                          MEMDB_IFTA100_MEM10_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM7_INDEX_DOP /* <253>
                                                          MEMDB_IFTA100_MEM7_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM5_INDEX_DOP /* <254>
                                                          MEMDB_IFTA100_MEM5_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM8_INDEX_DOP /* <255>
                                                          MEMDB_IFTA100_MEM8_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM2_INDEX_DOP /* <256>
                                                          MEMDB_IFTA100_MEM2_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM11_INDEX_DOP /* <257>
                                                          MEMDB_IFTA100_MEM11_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM6_INDEX_DOP /* <258>
                                                          MEMDB_IFTA100_MEM6_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM1_INDEX_DOP /* <259>
                                                          MEMDB_IFTA100_MEM1_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM4_INDEX_DOP /* <260>
                                                          MEMDB_IFTA100_MEM4_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM3_INDEX_DOP /* <261>
                                                          MEMDB_IFTA100_MEM3_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA100_MEM0_INDEX_DOP /* <262>
                                                          MEMDB_IFTA100_MEM0_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFSL100_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL100_LTS_TCAM_KEY_DOP /* <263>
                                                          IFSL100_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP /* <264>
                                                          ECMP_GROUP_LEVEL0_SHUFFLE_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP /* <265>
                                                          ECMP_GROUP_LEVEL0_GROUP_TABLE_DATA_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP /* <266>
                                                          ECMP_GROUP_LEVEL0_MEMBER_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA110_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA110_MY_DOP_INDEX_DOP /* <267>
                                                          MEMDB_IFTA110_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ETRAP_ETR_HIT_RTAG_HASH_DOP BCMPKT_TRACE_DOP_ID_ETRAP_ETR_HIT_RTAG_HASH_DOP /* <268>
                                                          ETRAP_ETR_HIT_RTAG_HASH_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ETRAP_ETR_OUT_DOP BCMPKT_TRACE_DOP_ID_ETRAP_ETR_OUT_DOP /* <269>
                                                          ETRAP_ETR_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP BCMPKT_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP /* <270>
                                                          DLB_ECMP_DLB_ECMP_CURRENT_TIME_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP BCMPKT_TRACE_DOP_ID_DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP /* <271>
                                                          DLB_ECMP_DLB_ECMP_FLOWSET_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP /* <272>
                                                          ECMP_GROUP_LEVEL1_SHUFFLE_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP /* <273>
                                                          ECMP_GROUP_LEVEL1_GROUP_TABLE_DATA_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP BCMPKT_TRACE_DOP_ID_ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP /* <274>
                                                          ECMP_GROUP_LEVEL1_MEMBER_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA120_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA120_MY_DOP_INDEX_DOP /* <275>
                                                          MEMDB_IFTA120_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA130_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA130_MY_DOP_INDEX_DOP /* <276>
                                                          MEMDB_IFTA130_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA130_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA130_I1T_00_INDEX_DOP /* <277>
                                                          IFTA130_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA130_I1T_01_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA130_I1T_01_INDEX_DOP /* <278>
                                                          IFTA130_I1T_01_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA130_I1T_02_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA130_I1T_02_INDEX_DOP /* <279>
                                                          IFTA130_I1T_02_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA130_I1T_03_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA130_I1T_03_INDEX_DOP /* <280>
                                                          IFTA130_I1T_03_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA140_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA140_MY_DOP_INDEX_DOP /* <281>
                                                          MEMDB_IFTA140_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA140_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA140_I1T_00_INDEX_DOP /* <282>
                                                          IFTA140_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP /* <283>
                                                          FLEX_CTR_ST_ING1_COUNTER_ACTION_VECTOR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP /* <284>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_1_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP /* <285>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_2_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP /* <286>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_3_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP /* <287>
                                                          FLEX_CTR_ST_ING1_COUNTER_POOL_0_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_B_DOP /* <288>
                                                          FLEX_CTR_ST_ING1_COUNTER_B_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP /* <289>
                                                          FLEX_CTR_ST_ING1_TRIGGER_STATE_CTRL_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP /* <290>
                                                          FLEX_CTR_ST_ING1_TRUTH_TABLE_CMD_OUTPUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_ING1_COUNTER_A_DOP /* <291>
                                                          FLEX_CTR_ST_ING1_COUNTER_A_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP /* <292>
                                                          IFTA150_T4T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP /* <293>
                                                          IFTA150_T4T_00_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP /* <294>
                                                          IFTA150_T4T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP /* <295>
                                                          IFTA150_T4T_00_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_TCAM_INDEX_DOP /* <296>
                                                          IFTA150_T4T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP /* <297>
                                                          IFTA150_T4T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFTA150_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_IFTA150_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <298>
                                                          IFTA150_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_1_KEY_DOP /* <299>
                                                          MEMDB_IFTA150_MEM0_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_3_KEY_DOP /* <300>
                                                          MEMDB_IFTA150_MEM0_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_2_KEY_DOP /* <301>
                                                          MEMDB_IFTA150_MEM0_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_0_KEY_DOP /* <302>
                                                          MEMDB_IFTA150_MEM0_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_IFTA150_MEM0_INDEX_DOP /* <303>
                                                          MEMDB_IFTA150_MEM0_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP BCMPKT_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP /* <304>
                                                          IPOST_DLB_LAG_DLB_LAG_CURRENT_TIME_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP /* <305>
                                                          IPOST_DLB_LAG_DLB_LAG_FLOWSET_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP /* <306>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_BUS_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP BCMPKT_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP /* <307>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_PRE_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP BCMPKT_TRACE_DOP_ID_IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP /* <308>
                                                          IPOST_MIRROR_SAMPLER_MIRROR_RESOLUTION_POST_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP /* <309>
                                                          IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP /* <310>
                                                          IPOST_QUEUE_ASSIGNMENT_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP /* <311>
                                                          IPOST_CPU_COS_CPU_COS_MAP_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP /* <312>
                                                          IPOST_CPU_COS_CPU_COS_MAP_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP /* <313>
                                                          IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP /* <314>
                                                          IPOST_MPB_ENCODE_ING_MPB_FLEX_DATA_SELECT_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP /* <315>
                                                          IPOST_MPB_ENCODE_MPB_FLEX_BUS_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP /* <316>
                                                          IPOST_MPB_CCBI_FIXED_CCBI_B_BUS_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP BCMPKT_TRACE_DOP_ID_IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP /* <317>
                                                          IPOST_MPB_CCBI_FIXED_MPB_FIXED_BUS_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP /* <318>
                                                          FLEX_CTR_ING_COUNTER_ACTION_VECTOR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP /* <319>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_2_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP /* <320>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_11_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP /* <321>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_8_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP /* <322>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_7_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP /* <323>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_5_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP /* <324>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_9_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP /* <325>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_6_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP /* <326>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_0_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP /* <327>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_1_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP /* <328>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_4_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP /* <329>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_3_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP /* <330>
                                                          FLEX_CTR_ING_COUNTER_EOP_BUFFER_10_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_IFSL140_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_IFSL140_LTS_TCAM_KEY_DOP /* <331>
                                                          IFSL140_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* <332>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_4_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* <333>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_1_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* <334>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_3_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* <335>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_2_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP /* <336>
                                                          EPRE_PARSER_ZONE_REMAP_ZONE_0_MATCH_ID_ATTRIBUTES_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP /* <337>
                                                          EPRE_PARSER_ZONE_REMAP_EGR_FIELD_EXTRACTION_PROFILE_CONTROL_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP /* <338>
                                                          EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER0_CTRL_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP BCMPKT_TRACE_DOP_ID_EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP /* <339>
                                                          EPRE_PARSER_ZONE_REMAP_EPRE2EPARSER1_CTRL_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP /* <340>
                                                          EPRE_EDEV_CONFIG_EGR_INT_CN_UPDATE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP /* <341>
                                                          EPRE_EDEV_CONFIG_FORWARDING_TYPE_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MPB_FIXED_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MPB_FIXED_DOP /* <342>
                                                          EPRE_EDEV_CONFIG_MPB_FIXED_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_CCBE_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_CCBE_DOP /* <343>
                                                          EPRE_EDEV_CONFIG_CCBE_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP /* <344>
                                                          EPRE_EDEV_CONFIG_EGR_TABLE_INDEX_UPDATE_PROFILE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP /* <345>
                                                          EPRE_EDEV_CONFIG_MIRROR_ATTRIBUTES_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP /* <346>
                                                          EPRE_MPB_DECODE_MPB_FLEX_MPB_PDD_PROFILE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_EFTA10_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_EFTA10_MY_DOP_INDEX_DOP /* <347>
                                                          MEMDB_EFTA10_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA10_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA10_I1T_00_INDEX_DOP /* <348>
                                                          EFTA10_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA10_I1T_01_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA10_I1T_01_INDEX_DOP /* <349>
                                                          EFTA10_I1T_01_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA10_I1T_02_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA10_I1T_02_INDEX_DOP /* <350>
                                                          EFTA10_I1T_02_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA10_I1T_03_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA10_I1T_03_INDEX_DOP /* <351>
                                                          EFTA10_I1T_03_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_EFTA20_MY_DOP_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_EFTA20_MY_DOP_INDEX_DOP /* <352>
                                                          MEMDB_EFTA20_MY_DOP_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA20_I1T_00_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA20_I1T_00_INDEX_DOP /* <353>
                                                          EFTA20_I1T_00_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA20_I1T_01_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA20_I1T_01_INDEX_DOP /* <354>
                                                          EFTA20_I1T_01_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA20_I1T_02_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA20_I1T_02_INDEX_DOP /* <355>
                                                          EFTA20_I1T_02_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA20_I1T_03_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA20_I1T_03_INDEX_DOP /* <356>
                                                          EFTA20_I1T_03_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA20_I1T_04_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA20_I1T_04_INDEX_DOP /* <357>
                                                          EFTA20_I1T_04_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA20_I1T_05_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA20_I1T_05_INDEX_DOP /* <358>
                                                          EFTA20_I1T_05_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA20_I1T_06_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA20_I1T_06_INDEX_DOP /* <359>
                                                          EFTA20_I1T_06_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFSL20_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EFSL20_LTS_TCAM_KEY_DOP /* <360>
                                                          EFSL20_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP /* <361>
                                                          EFTA30_E2T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP /* <362>
                                                          EFTA30_E2T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_TCAM_INDEX_DOP /* <363>
                                                          EFTA30_E2T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP /* <364>
                                                          EFTA30_E2T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_EFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <365>
                                                          EFTA30_E2T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP /* <366>
                                                          EFTA30_T4T_00_LTS_TCAM_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP /* <367>
                                                          EFTA30_T4T_00_LTS_TCAM_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP /* <368>
                                                          EFTA30_T4T_00_LTS_TCAM_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP /* <369>
                                                          EFTA30_T4T_00_LTS_TCAM_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_TCAM_INDEX_DOP /* <370>
                                                          EFTA30_T4T_00_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP /* <371>
                                                          EFTA30_T4T_00_LTS_HIT_CONTEXT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFTA30_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP BCMPKT_TRACE_DOP_ID_EFTA30_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP /* <372>
                                                          EFTA30_T4T_00_LTS_PRE_SEL_MUX_OUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP BCMPKT_TRACE_DOP_ID_EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP /* <373>
                                                          EGR_TIMESTAMP_TIMESTAMP_COUNT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP BCMPKT_TRACE_DOP_ID_EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP /* <374>
                                                          EGR_MEMBERSHIP_MEMBERSHIP_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP /* <375>
                                                          FLEX_CTR_ST_EGR_COUNTER_ACTION_VECTOR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP /* <376>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_1_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP /* <377>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_2_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP /* <378>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_3_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP /* <379>
                                                          FLEX_CTR_ST_EGR_COUNTER_POOL_0_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_B_DOP /* <380>
                                                          FLEX_CTR_ST_EGR_COUNTER_B_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP /* <381>
                                                          FLEX_CTR_ST_EGR_TRIGGER_STATE_CTRL_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR_COUNTER_A_DOP /* <382>
                                                          FLEX_CTR_ST_EGR_COUNTER_A_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP /* <383>
                                                          FLEX_CTR_ST_EGR0_COUNTER_ACTION_VECTOR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_1_UPDATE_CMD_DOP /* <384>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_1_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_2_UPDATE_CMD_DOP /* <385>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_2_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_3_UPDATE_CMD_DOP /* <386>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_3_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_POOL_0_UPDATE_CMD_DOP /* <387>
                                                          FLEX_CTR_ST_EGR0_COUNTER_POOL_0_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_B_DOP /* <388>
                                                          FLEX_CTR_ST_EGR0_COUNTER_B_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP /* <389>
                                                          FLEX_CTR_ST_EGR0_TRIGGER_STATE_CTRL_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_COUNTER_A_DOP /* <390>
                                                          FLEX_CTR_ST_EGR0_COUNTER_A_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP /* <391>
                                                          FLEX_CTR_ST_EGR0_TRUTH_TABLE_CMD_OUTPUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP /* <392>
                                                          FLEX_CTR_ST_EGR1_COUNTER_ACTION_VECTOR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_1_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_1_UPDATE_CMD_DOP /* <393>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_1_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_2_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_2_UPDATE_CMD_DOP /* <394>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_2_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_3_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_3_UPDATE_CMD_DOP /* <395>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_3_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_0_UPDATE_CMD_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_POOL_0_UPDATE_CMD_DOP /* <396>
                                                          FLEX_CTR_ST_EGR1_COUNTER_POOL_0_UPDATE_CMD_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_B_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_B_DOP /* <397>
                                                          FLEX_CTR_ST_EGR1_COUNTER_B_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP /* <398>
                                                          FLEX_CTR_ST_EGR1_TRIGGER_STATE_CTRL_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_A_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_COUNTER_A_DOP /* <399>
                                                          FLEX_CTR_ST_EGR1_COUNTER_A_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP /* <400>
                                                          FLEX_CTR_ST_EGR1_TRUTH_TABLE_CMD_OUTPUT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP /* <401>
                                                          MEMDB_TCAM_EFTA30_MEM0_1_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP /* <402>
                                                          MEMDB_TCAM_EFTA30_MEM0_3_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP /* <403>
                                                          MEMDB_TCAM_EFTA30_MEM0_2_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP /* <404>
                                                          MEMDB_TCAM_EFTA30_MEM0_0_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP BCMPKT_TRACE_DOP_ID_MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP /* <405>
                                                          MEMDB_TCAM_EFTA30_MEM0_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EFSL30_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EFSL30_LTS_TCAM_KEY_DOP /* <406>
                                                          EFSL30_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP BCMPKT_TRACE_DOP_ID_EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP /* <407>
                                                          EGR_MIRROR_MIRROR2EDIT_CTRL_BUS_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP /* <408>
                                                          QOS_REMARKING_LTS_TCAM_ONLY_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP /* <409>
                                                          QOS_REMARKING_LTS_TCAM_HIT_AND_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_KEY_DOP /* <410>
                                                          QOS_REMARKING_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_LTS_TCAM_INDEX_DOP /* <411>
                                                          QOS_REMARKING_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP /* <412>
                                                          QOS_REMARKING_QOS_MAP_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_KEY_DOP /* <413>
                                                          EDIT_CTRL_ZONE_4_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_KEY_DOP /* <414>
                                                          EDIT_CTRL_ZONE_3_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_KEY_DOP /* <415>
                                                          EDIT_CTRL_ZONE_1_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_KEY_DOP /* <416>
                                                          EDIT_CTRL_ZONE_2_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_KEY_DOP /* <417>
                                                          EDIT_CTRL_ZONE_0_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_TCAM_HIT_DOP /* <418>
                                                          EDIT_CTRL_ZONE_0_TCAM_HIT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_TCAM_HIT_DOP /* <419>
                                                          EDIT_CTRL_ZONE_4_TCAM_HIT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_TCAM_HIT_DOP /* <420>
                                                          EDIT_CTRL_ZONE_3_TCAM_HIT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_TCAM_HIT_DOP /* <421>
                                                          EDIT_CTRL_ZONE_2_TCAM_HIT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_TCAM_HIT_DOP /* <422>
                                                          EDIT_CTRL_ZONE_1_TCAM_HIT_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_LTS_TCAM_KEY_DOP /* <423>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_LTS_TCAM_KEY_DOP /* <424>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_LTS_TCAM_KEY_DOP /* <425>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_LTS_TCAM_KEY_DOP /* <426>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_LTS_TCAM_KEY_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_LTS_TCAM_KEY_DOP /* <427>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_KEY_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP /* <428>
                                                          EDIT_CTRL_ZONE_0_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP /* <429>
                                                          EDIT_CTRL_ZONE_1_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP /* <430>
                                                          EDIT_CTRL_ZONE_2_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP /* <431>
                                                          EDIT_CTRL_ZONE_3_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP /* <432>
                                                          EDIT_CTRL_ZONE_4_LTS_TCAM_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP /* <433>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_2_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP /* <434>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_2_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP /* <435>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_RW_1_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP /* <436>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_1_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP /* <437>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_1_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP /* <438>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MIRROR_0_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP /* <439>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_3_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP /* <440>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_RW_0_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP /* <441>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_0_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP /* <442>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_SMALL_FLEX_HDR_0_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP /* <443>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_LARGE_FLEX_HDR_0_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP /* <444>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_2_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP BCMPKT_TRACE_DOP_ID_EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP /* <445>
                                                          EDIT_CTRL_EDIT_CTRL2EDITOR_MHC_CHECKSUM_1_PROFILE_PTR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP /* <446>
                                                          EGR_SEQUENCE_MIRROR_SEQUENCE_NUMBER_PROFILE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP /* <447>
                                                          EGR_SEQUENCE_SEQUENCE_NUMBER_TABLE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP /* <448>
                                                          EGR_SEQUENCE_SEQUENCE_PROFILE_INDEX_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP /* <449>
                                                          EGR_SEQUENCE_NEW_SEQUENCE_NUM_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP BCMPKT_TRACE_DOP_ID_EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP /* <450>
                                                          EGR_SEQUENCE_PKT_SEQUENCE_NUM_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_EDITOR_MATCH_ID_DOP BCMPKT_TRACE_DOP_ID_FLEX_EDITOR_MATCH_ID_DOP /* <451>
                                                          FLEX_EDITOR_MATCH_ID_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_EDITOR_VHLEN_DOP BCMPKT_TRACE_DOP_ID_FLEX_EDITOR_VHLEN_DOP /* <452>
                                                          FLEX_EDITOR_VHLEN_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_EDITOR_EDIT_ID_DOP BCMPKT_TRACE_DOP_ID_FLEX_EDITOR_EDIT_ID_DOP /* <453>
                                                          FLEX_EDITOR_EDIT_ID_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP /* <454>
                                                          FLEX_CTR_EGR_COUNTER_ACTION_VECTOR_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP /* <455>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_2_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP /* <456>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_7_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP /* <457>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_5_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP /* <458>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_6_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP /* <459>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_0_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP /* <460>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_1_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP /* <461>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_4_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP BCMPKT_TRACE_DOP_ID_FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP /* <462>
                                                          FLEX_CTR_EGR_COUNTER_EOP_BUFFER_3_DOP. */
#define BCM_PKTIO_TRACE_DOP_ID_COUNT        BCMPKT_TRACE_DOP_ID_COUNT /* <463> TRACE_DOP_ID
                                                          FIELD ID NUMBER */

#define BCM_PKTIO_LBHDR_FID_INVALID BCMPKT_LBHDR_FID_INVALID /* Invalid BCMPKT_LBHDR FID
                                                  indicator */

#define BCM_PKTIO_LBHDR_START   BCMPKT_LBHDR_START /* Start of frame indicator (must be
                                              set to 0xFB) */

#define BCM_PKTIO_LBHDR_HEADER_TYPE BCMPKT_LBHDR_HEADER_TYPE /* 64 Header Types Supported. 1
                                                  means packets from CPU with
                                                  SOBMH header format to
                                                  Passthru NLF. */

#define BCM_PKTIO_LBHDR_INPUT_PRIORITY  BCMPKT_LBHDR_INPUT_PRIORITY /* Indicates the loopback COS
                                                      queue and is used by the
                                                      MMU for per application
                                                      group accounting for
                                                      packets received from the
                                                      internal loopback port.
                                                      This must be put the
                                                      PBI.INPUT_PRIORITY
                                                      whenever the source port
                                                      is the loopback port. */

#define BCM_PKTIO_LBHDR_PKT_PROFILE BCMPKT_LBHDR_PKT_PROFILE /* Pointer to
                                                  CPU_PACKET_PROFILE_1/2
                                                  registers. */

#define BCM_PKTIO_LBHDR_VISIBILITY_PKT  BCMPKT_LBHDR_VISIBILITY_PKT /* Triggers capture of
                                                      intermediate packet
                                                      processing result of this
                                                      packet. */

#define BCM_PKTIO_LBHDR_SOURCE  BCMPKT_LBHDR_SOURCE /* Source field for the packet.
                                              SOURCE_TYPE=0x0: [15:0] SOURCE_VP.
                                              SOURCE_TYPE=0x1: [15:8] SRC_MODID,
                                              [7:0] SRC_PORT. */

#define BCM_PKTIO_LBHDR_SOURCE_TYPE BCMPKT_LBHDR_SOURCE_TYPE /* Indicates contents of SOURCE
                                                  field. 0x1: SGPP 0x0: SVP. */

#define BCM_PKTIO_LBHDR_TRILL_AC_CNG    BCMPKT_LBHDR_TRILL_AC_CNG /* CNG Bits */

#define BCM_PKTIO_LBHDR_TRILL_AC_TC BCMPKT_LBHDR_TRILL_AC_TC /* Traffic class */

#define BCM_PKTIO_LBHDR_TRILL_AC_DO_NOT_LEARN BCMPKT_LBHDR_TRILL_AC_DO_NOT_LEARN /* Do Not Learn Bit */

#define BCM_PKTIO_LBHDR_TRILL_NW_CNG    BCMPKT_LBHDR_TRILL_NW_CNG /* CNG Bits */

#define BCM_PKTIO_LBHDR_TRILL_NW_TC BCMPKT_LBHDR_TRILL_NW_TC /* Traffic class */

#define BCM_PKTIO_LBHDR_ETH_DEST_TYPE   BCMPKT_LBHDR_ETH_DEST_TYPE /* Destination type (see
                                                      encodings). Valid only if
                                                      SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_DEST    BCMPKT_LBHDR_ETH_DEST /* Destination based on
                                                  DESTINATION_TYPE. (For
                                                  ECMP/ECMP_MEMBER cases, this
                                                  field contains
                                                  ECMP_GROUP_PTR). Valid only if
                                                  SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_DP  BCMPKT_LBHDR_ETH_DP /* Drop precedence for the apcket.
                                              Valid only if
                                              SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_ECMP_MEMBER_ID  BCMPKT_LBHDR_ETH_ECMP_MEMBER_ID /* ECMP member ID for
                                                          case where
                                                          DESTINATION_TYPE=ECMP_MEMBER.
                                                          Only valid for single
                                                          level ECMP. Valid only
                                                          if
                                                          SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_INT_CN  BCMPKT_LBHDR_ETH_INT_CN /* Internal Congestion to be used
                                                  for the packet. Valid only if
                                                  SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_INT_PRI BCMPKT_LBHDR_ETH_INT_PRI /* Internal priority to be used
                                                  for the packet. Valid only if
                                                  SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_MCAST_LB_INDEX  BCMPKT_LBHDR_ETH_MCAST_LB_INDEX /* Allows software to
                                                          select load balancing
                                                          bitmap for non-unicast
                                                          packets. Valid only if
                                                          MCAST_LB_INDEX_VLD=1. */

#define BCM_PKTIO_LBHDR_ETH_MCAST_LB_INDEX_VLD BCMPKT_LBHDR_ETH_MCAST_LB_INDEX_VLD /* If set, MCAST_LB_INDEX
                                                          field is valid. */

#define BCM_PKTIO_LBHDR_ETH_PP_PORT BCMPKT_LBHDR_ETH_PP_PORT /* Device Port Number which is to
                                                  be used for processing packet
                                                  in Ingress Pipeline. */

#define BCM_PKTIO_LBHDR_ETH_QOS_FIELDS_VLD  BCMPKT_LBHDR_ETH_QOS_FIELDS_VLD /* If set, the qos fields
                                                          (INT, PRI, etc) are
                                                          specified in the
                                                          header and override
                                                          the pre-IFP
                                                          assignment. Else,
                                                          derived based on
                                                          packet lookups. Valid
                                                          only if
                                                          SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_ROUTED_PKT  BCMPKT_LBHDR_ETH_ROUTED_PKT /* Indicates whether packets
                                                      should be treated as
                                                      routed or bridged. Valid
                                                      only if
                                                      SUBFLOW_TYPE=CPU_TX_PROC. */

#define BCM_PKTIO_LBHDR_ETH_SUBFLOW_TYPE    BCMPKT_LBHDR_ETH_SUBFLOW_TYPE /* Identifies sub-flow
                                                          (see encodings) */

#define BCM_PKTIO_LBHDR_ETHERNET_VRF    BCMPKT_LBHDR_ETHERNET_VRF /* VRF */

#define BCM_PKTIO_LBHDR_ETHERNET_VRF_VALID  BCMPKT_LBHDR_ETHERNET_VRF_VALID /* VRF valid */

#define BCM_PKTIO_LBHDR_FID_COUNT   BCMPKT_LBHDR_FID_COUNT /* LBHDR field ID number */

#define BCM_PKTIO_LBHDR_HEADER_T_TUNNEL_TERM BCMPKT_LBHDR_HEADER_T_TUNNEL_TERM /* MinM, L2GRE and VXLAN
                                                          tunnel terminated
                                                          packet */

#define BCM_PKTIO_LBHDR_HEADER_T_TRILL_NW   BCMPKT_LBHDR_HEADER_T_TRILL_NW /* Trill Network Packets
                                                          to Passthru NLF */

#define BCM_PKTIO_LBHDR_HEADER_T_TRILL_AC   BCMPKT_LBHDR_HEADER_T_TRILL_AC /* Trill Access Layer
                                                          Packets to Passthru
                                                          NLF */

#define BCM_PKTIO_LBHDR_HEADER_T_ETHERNET   BCMPKT_LBHDR_HEADER_T_ETHERNET /* Process as if it was
                                                          received on front
                                                          panel port */

#define BCM_PKTIO_LBHDR_HEADER_T_MIM    BCMPKT_LBHDR_HEADER_T_MIM /* MAC in MAC packets to
                                                      Passthru NLF */

#define BCM_PKTIO_LBHDR_HEADER_T_QCN    BCMPKT_LBHDR_HEADER_T_QCN /* QCN Packets to Passthru
                                                      NLF */

#define BCM_PKTIO_LBHDR_HEADER_T_GENERIC    BCMPKT_LBHDR_HEADER_T_GENERIC /* Generic loopback */

#define BCM_PKTIO_LBHDR_HEADER_T_TRANSPORT  BCMPKT_LBHDR_HEADER_T_TRANSPORT /* Transport header */

#define BCM_PKTIO_LBHDR_ETH_DEST_T_DGLP BCMPKT_LBHDR_ETH_DEST_T_DGLP /* Destination GLP. Note that
                                                      despite the name, only
                                                      physical ports (DGPP) are
                                                      supported. Trunking is not
                                                      supported. */

#define BCM_PKTIO_LBHDR_ETH_DEST_T_NHI  BCMPKT_LBHDR_ETH_DEST_T_NHI /* Next hop */

#define BCM_PKTIO_LBHDR_ETH_DEST_T_ECMP BCMPKT_LBHDR_ETH_DEST_T_ECMP /* ECMP group */

#define BCM_PKTIO_LBHDR_ETH_DEST_T_ECMP_MEMBER BCMPKT_LBHDR_ETH_DEST_T_ECMP_MEMBER /* ECMP member */

#define BCM_PKTIO_LBHDR_ETH_DEST_T_IPMC BCMPKT_LBHDR_ETH_DEST_T_IPMC /* IPMC */

#define BCM_PKTIO_LBHDR_ETH_DEST_T_L2MC BCMPKT_LBHDR_ETH_DEST_T_L2MC /* L2MC */

#define BCM_PKTIO_LBHDR_ETH_DEST_T_VLAN_FLOOD BCMPKT_LBHDR_ETH_DEST_T_VLAN_FLOOD /* Vlan flooding */

#define BCM_PKTIO_LBHDR_ETH_SUBFLOW_T_CPU_MASQUERADE BCMPKT_LBHDR_ETH_SUBFLOW_T_CPU_MASQUERADE /* CPU masquerade flow */

#define BCM_PKTIO_LBHDR_ETH_SUBFLOW_T_CPU_TX_PROC BCMPKT_LBHDR_ETH_SUBFLOW_T_CPU_TX_PROC /* CPU_TX_PROC */

#define BCM_PKTIO_LBHDR_I_FID_INVALID   BCMPKT_LBHDR_I_FID_INVALID /* Invalid BCMPKT_LBHDR_I FID
                                                      indicator */

#define BCM_PKTIO_LBHDR_I_SIZE  BCMPKT_LBHDR_I_SIZE /* LBHDR RX raw data size */

#define BCM_PKTIO_LBHDR_I_FID_COUNT BCMPKT_LBHDR_I_FID_COUNT /* LBHDR_I field ID number */

#endif /* defined(INCLUDE_PKTIO) || defined(BCM_LTSW_SUPPORT) || defined(BCM_CTSW_SUPPORT) */

#endif /* __BCM_PKTIO_DEFS_H__ */
