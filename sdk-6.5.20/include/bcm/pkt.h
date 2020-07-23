/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 */

#ifndef __BCM_PKT_H__
#define __BCM_PKT_H__

#include <shared/rx.h>
#include <shared/pkt.h>
#include <bcm/types.h>
#include <bcm/vlan.h>

/* Default TX and RX DMA channels for the BCM layer. */
#define BCM_TX_CHAN_DFLT        0          
#define BCM_RX_CHAN_DFLT        1          

/* Minimum packet allocation:  MAC addresses, VLAN tag, CRC. */
#define BCM_PKT_ALLOC_MIN       (12 + 4 + 4) 

/* 
 * IEEE header:  MAC addresses + VLAN tag; does not include type/len
 * field.
 */
#define BCM_IEEE_HDR_BYTES      (12 + 4)   

/* 
 * The packet structure. The packet layout is as follows:
 * 
 *   DMAC + SMAC     12 bytes
 *   VLAN tag         4 bytes (may be filled in by SW on BCM5670/75)
 *   payload          N bytes
 *   CRC              4 bytes
 *   pad              M bytes
 *   SL tag           4 bytes (may be unused)
 *   HiGig Header    12 bytes (may be unused)
 * 
 * The rule is: alloc_len = 12 + 4 + N + 4 + M + 4 + 12 (all of above).
 * payload_len (below) is N.
 * 
 * Note that the payload may grow until M == 0; the CRC moves. The SL and
 * HiGig headers will not move.
 * 
 * The "IEEE packet" is everything from the DMAC through the CRC
 * (inclusive), not including SL tag or HiGig header.
 * 
 * Scatter/gather is used to put the data into the right positions on
 * transmit and receive. The SL/HiGig headers are parsed on RX into data
 * members in the packet structure. On TX, bcm_tx will send the packet
 * according to the unit type. It will not check or affect any fields
 * except maybe the CRC. Other routines will be provided to ensure the
 * HiGig and SL tags are properly set up from the data in the packet
 * structure.
 */
typedef struct bcm_pkt_s bcm_pkt_t;

/* bcm_pkt_cb_f */
typedef void (*bcm_pkt_cb_f)(
    int unit, 
    bcm_pkt_t *pkt, 
    void *cookie);

/* BCM packet gather block type. */
typedef struct bcm_pkt_blk_s {
    uint8 *data; 
    int len; 
} bcm_pkt_blk_t;

/* Set of 'reasons' (see bcmRxReason*) why a packet came to the CPU. */
typedef _shr_rx_reasons_t bcm_rx_reasons_t;

/* Stacking header packet forwarding options. */
typedef enum bcm_pkt_stk_forward_e {
    BCM_PKT_STK_FORWARD_CPU = 0,        /* Stacking header packet forwarding
                                           option: to Host CPU. */
    BCM_PKT_STK_FORWARD_L2_UNICAST = 1, /* Stacking header packet forwarding
                                           option: Unicast L2. */
    BCM_PKT_STK_FORWARD_L3_UNICAST = 2, /* Stacking header packet forwarding
                                           option: Unicast L3. */
    BCM_PKT_STK_FORWARD_L2_MULTICAST = 3, /* Stacking header packet forwarding
                                           option: Multicast L2. */
    BCM_PKT_STK_FORWARD_L2_MULTICAST_UNKNOWN = 4, /* Stacking header packet forwarding
                                           option: Unknown Multicast L2. */
    BCM_PKT_STK_FORWARD_L3_MULTICAST = 5, /* Stacking header packet forwarding
                                           option: Multicast L3. */
    BCM_PKT_STK_FORWARD_L3_MULTICAST_UNKNOWN = 6, /* Stacking header packet forwarding
                                           option: Unknown Multicast L3. */
    BCM_PKT_STK_FORWARD_L2_UNICAST_UNKNOWN = 7, /* Stacking header packet forwarding
                                           option: Unknown Unicast L2. */
    BCM_PKT_STK_FORWARD_BROADCAST = 8,  /* Stacking header packet forwarding
                                           option: Broadcast. */
    BCM_PKT_STK_FORWARD_MPLS = 9,       /* Stacking header packet forwarding
                                           option: MPLS. */
    BCM_PKT_STK_FORWARD_TRILL = 10,     /* Stacking header packet forwarding
                                           option: TRILL. */
    BCM_PKT_STK_FORWARD_FCOE = 11,      /* Stacking header packet forwarding
                                           option: FCoE. */
    BCM_PKT_STK_FORWARD_SNOOP = 12,     /* Stacking header packet forwarding
                                           option: Snoop. */
    BCM_PKT_STK_FORWARD_TRAFFIC_MANAGEMENT = 13, /* Stacking header packet forwarding
                                           option: Traffic Management. */
    BCM_PKT_STK_FORWARD_COUNT = 14      /* Must be last. */
} bcm_pkt_stk_forward_t;

/* Decap Tunnel Types */
typedef enum bcm_rx_decap_tunnel_e {
    bcmRxDecapNone = _SHR_RX_DECAP_NONE, /* No tunnel Decap */
    bcmRxDecapAccessSVP = _SHR_RX_DECAP_ACCESS_SVP, /* Packet ingress on Access SVP (No
                                           decap) */
    bcmRxDecapMIM = _SHR_RX_DECAP_MIM,  /* Decap Mac-in-Mac tunnel */
    bcmRxDecapL2GRE = _SHR_RX_DECAP_L2GRE, /* Decap L2GRE tunnel */
    bcmRxDecapVXLAN = _SHR_RX_DECAP_VXLAN, /* Decap VXLAN tunnel */
    bcmRxDecapAMT = _SHR_RX_DECAP_AMT,  /* Decap AMT tunnel */
    bcmRxDecapIP = _SHR_RX_DECAP_IP,    /* Decap IP tunnel */
    bcmRxDecapTRILL = _SHR_RX_DECAP_TRILL, /* Decap TRILL tunnel */
    bcmRxDecapMPLS1LABEL = _SHR_RX_DECAP_MPLS_1LABEL, /* Decap MPLS 1 Label, no Control Word */
    bcmRxDecapL2MPLS1LABEL = _SHR_RX_DECAP_L2MPLS_1LABEL, /* Decap MPLS 1 Label, L2 payload, no
                                           Control Word */
    bcmRxDecapMPLS2LABEL = _SHR_RX_DECAP_MPLS_2LABEL, /* Decap MPLS 2 Label, no Control Word */
    bcmRxDecapL2MPLS2LABEL = _SHR_RX_DECAP_L2MPLS_2LABEL, /* Decap MPLS 2 Label, L2 payload, no
                                           Control Word */
    bcmRxDecapMPLS1LABELCW = _SHR_RX_DECAP_MPLS_1LABELCW, /* Decap MPLS 1 Label, Control Word
                                           present */
    bcmRxDecapL2MPLS1LABELCW = _SHR_RX_DECAP_L2MPLS_1LABELCW, /* Decap MPLS 1 Label, L2 payload,
                                           Control Word present */
    bcmRxDecapMPLS2LABELCW = _SHR_RX_DECAP_MPLS_2LABELCW, /* Decap MPLS 2 Label, Control Word
                                           present */
    bcmRxDecapL2MPLS2LABELCW = _SHR_RX_DECAP_L2MPLS_2LABELCW, /* Decap MPLS 2 Label, L2 payload,
                                           Control Word present */
    bcmRxDecapL3MPLS1LABEL = _SHR_RX_DECAP_L3MPLS_1LABEL, /* Decap MPLS 1 Label, L3 payload, no
                                           Control Word present */
    bcmRxDecapL3MPLS2LABEL = _SHR_RX_DECAP_L3MPLS_2LABEL, /* Decap MPLS 2 Label, L3 payload, no
                                           Control Word present */
    bcmRxDecapL3MPLS1LABELCW = _SHR_RX_DECAP_L3MPLS_1LABELCW, /* Decap MPLS 1 Label, L3 payload,
                                           Control Word present */
    bcmRxDecapL3MPLS2LABELCW = _SHR_RX_DECAP_L3MPLS_2LABELCW, /* Decap MPLS 2 Label, L3 payload,
                                           Control Word present */
    bcmRxDecapWTP2AC = _SHR_RX_DECAP_WTP2AC, /* Decap WTP2AC Tunnel */
    bcmRxDecapAC2AC = _SHR_RX_DECAP_AC2AC, /* Decap AC2AC Tunnel */
    bcmRxDecapMPLS3LABEL = _SHR_RX_DECAP_MPLS_3LABEL, /* Decap MPLS 3 Label, no Control Word
                                           present */
    bcmRxDecapMPLS3LABELCW = _SHR_RX_DECAP_MPLS_3LABELCW, /* Decap MPLS 3 Label, Control Word
                                           present */
    bcmRxDecapMPLS3LABELENTROPYLABEL = _SHR_RX_DECAP_MPLS_3LABEL_ENTROPY, /* Decap MPLS 3 Label + entropy, no
                                           Control Word */
    bcmRxDecapMPLS3LABELENTROPYLABELCW = _SHR_RX_DECAP_MPLS_3LABEL_ENTROPYCW, /* Decap MPLS 3 Label + entropy, Control
                                           Word present */
    bcmRxDecapCount = _SHR_RX_DECAP_COUNT /* Decap Tunnel Max */
} bcm_rx_decap_tunnel_t;

/* Packet Headers Decapped after Ingress */
typedef enum bcm_rx_decap_hdr_e {
    bcmRxDecapHdrNone = _SHR_RX_DECAP_HDR_NONE, /* No headers Decapped */
    bcmRxDecapSystemHdr = _SHR_RX_DECAP_SYSTEM_HDR, /* Decap System Header */
    bcmRxDecapOuterL2Hdr = _SHR_RX_DECAP_OUTER_L2_HDR, /* Decap Outer L2 Header */
    bcmRxDecapL3L4TunnelHdr = _SHR_RX_DECAP_L3_L4_TUNNEL_HDR, /* Decap Outer L3/L4/TUNNEL Header */
    bcmRxDecapInnerL2Hdr = _SHR_RX_DECAP_INNER_L2_HDR, /* Decap Inner L2 Header */
    bcmRxDecapInnerL3L4Hdr = _SHR_RX_DECAP_INNER_L3_L4_HDR /* Decap Inner L3 and L4 Header */
} bcm_rx_decap_hdr_t;

/* OAM DM timestamp options. */
typedef enum bcm_pkt_timestamp_mode_e {
    BCM_PKT_TIMESTAMP_MODE_NONE = 0,    /* No Timestamp */
    BCM_PKT_TIMESTAMP_MODE_PTP = 1,     /* Timestamp mode - PTP. */
    BCM_PKT_TIMESTAMP_MODE_NTP = 2,     /* Timestamp mode - NTP. */
    BCM_PKT_TIMESTAMP_MODE_NUM = 3      /* Max value - Not to be used */
} bcm_pkt_timestamp_mode_t;

/* OAM LM counter options. */
typedef enum bcm_pkt_oam_lm_counter_mode_e {
    BCM_PKT_OAM_LM_COUNTER_MODE_NONE = 0, /* No LM counter operation */
    BCM_PKT_OAM_LM_COUNTER_MODE_INCREMENT = 1, /* Increment LM counter. */
    BCM_PKT_OAM_LM_COUNTER_MODE_SAMPLE = 2, /* Sample LM counter. */
    BCM_PKT_OAM_LM_COUNTER_MODE_NUM = 3 /* Max value - Not to be used */
} bcm_pkt_oam_lm_counter_mode_t;

/* OAM Pkt Type */
typedef enum bcm_pkt_rx_oam_type_e {
    bcmPktRxOamTypeNone = 0,            /* Not OAM pkt */
    bcmPktRxOamTypeBfdOam = 1,          /* BFD OAM packet */
    bcmPktRxOamTypeEthOamCcm = 2,       /* Ethernet OAM Down MEP CCM packet */
    bcmPktRxOamTypeEthOamLm = 3,        /* Ethernet OAM Down MEP LM packet */
    bcmPktRxOamTypeEthOamDm = 4,        /* Ethernet OAM Down MEP DM packet */
    bcmPktRxOamTypeEthOamOther = 5,     /* Ethernet OAM Down MEP other opcode
                                           packet */
    bcmPktRxOamTypeBhhOamCcm = 6,       /* BHH OAM CCM packet */
    bcmPktRxOamTypeBhhOamLm = 7,        /* BHH OAM LM packet */
    bcmPktRxOamTypeBhhOamDm = 8,        /* BHH OAM DM packet */
    bcmPktRxOamTypeBhhOamOther = 9,     /* BHH OAM other opcode packet */
    bcmPktRxOamTypeRfc6374Dlm = 10,     /* MPLS LM/DM  DLM packet */
    bcmPktRxOamTypeRfc6374Dm = 11,      /* MPLS LM/DM  DM packet */
    bcmPktRxOamTypeRfc6374DlmPlusDm = 12, /* MPLS LM/DM  DLM + DM packet */
    bcmPktRxOamTypeRfc6374Ilm = 13,     /* MPLS LM/DM  ILM packet */
    bcmPktRxOamTypeRfc6374IlmPlusDm = 14, /* MPLS LM/DM  ILM + DM packet */
    bcmPktRxOamTypeSat = 15,            /* Service Activation Test */
    bcmPktRxOamTypeOtherAch = 16,       /* OAM packets with other ACH types */
    bcmPktRxOamTypeEthOamUpMepCcm = 17, /* Ethernet OAM Up MEP CCM packet */
    bcmPktRxOamTypeEthOamUpMepLm = 18,  /* Ethernet OAM Up MEP LM packet */
    bcmPktRxOamTypeEthOamUpMepDm = 19,  /* Ethernet OAM Up MEP DM packet */
    bcmPktRxOamTypeEthOamUpMepOther = 20, /* Ethernet OAM Up MEP other opcode
                                           packet */
    bcmPktRxOamTypeUpSat = 21,          /* Up Service Activation Test packet */
    bcmPktRxOamTypeCount = 22           /* Max value - Not to be used */
} bcm_pkt_rx_oam_type_t;

/* OAM counter object ID */
typedef enum bcm_pkt_oam_counter_object_e {
    bcmOamCounterObjectNone = 0,        /* Invalid counter object */
    bcmOamCounterObjectEndpointId = 1,  /* OAM counter object id */
    bcmOamCounterObjectFlexStatId = 2   /* OAM counter flex stats ID */
} bcm_pkt_oam_counter_object_t;

/* Indicates how the packet was forwarded. */
typedef enum bcm_pkt_forwarding_type_e {
    BCM_PKT_L2_FORWARD = 0, /* Ethernet/NIV/HiGig forwarded. */
    BCM_PKT_L3UC = 1,       /* L3 UC forwarded. */
    BCM_PKT_L2L3 = 2        /* Bridge routed. */
} bcm_pkt_forwarding_type_t;

/* OAM counter */
typedef struct bcm_pkt_oam_counter_s {
    bcm_pkt_oam_counter_object_t counter_object; /* OAM counter object type */
    uint32 counter_object_id;           /* Counter Object Id */
    uint32 counter_offset;              /* Offset from start of counter group */
    bcm_pkt_oam_lm_counter_mode_t counter_mode; /* Counter mode during Tx */
    uint32 counter_value_upper;         /* Upper 32 bit of OAM LM Counter in Rx
                                           Pkt */
    uint32 counter_value_lower;         /* Lower 32 bit of OAM LM Counter in Rx
                                           Pkt */
    uint32 oam_lm_byte_count_offset;    /* Offset to start byte counting */
} bcm_pkt_oam_counter_t;

/* OAM counter max object */
#define BCM_PKT_OAM_COUNTER_MAX 3          

/* Rx path definitions */
#define BCM_RX_PATH_SWITCHED    (1 << 0)   
#define BCM_RX_PATH_COPY_TO_CPU (1 << 1)   
#define BCM_RX_PATH_MIRRORED    (1 << 2)   

/* DNX internal header stack array size */
#define BCM_PKT_NOF_DNX_HEADERS _SHR_PKT_NOF_DNX_HEADERS 

/* Pkt DNX types. */
typedef enum bcm_pkt_dnx_type_e {
    bcmPktDnxTypeItmh = _SHR_PKT_DNX_TYPE_ITMH, /* ITMH Header. */
    bcmPktDnxTypeFtmh = _SHR_PKT_DNX_TYPE_FTMH, /* FMTH Header. */
    bcmPktDnxTypeOtsh = _SHR_PKT_DNX_TYPE_OTSH, /* OAM-TS Header (OTSH). */
    bcmPktDnxTypeOtmh = _SHR_PKT_DNX_TYPE_OTMH, /* OTMH Header. */
    bcmPktDnxTypeInternals = _SHR_PKT_DNX_TYPE_INTERNALS /* Dnx Internal fields. */
} bcm_pkt_dnx_type_t;

/* itmh dest type. */
typedef enum bcm_pkt_dnx_itmh_dest_type_e {
    bcmPktDnxItmhDestTypeMulticast = _SHR_PKT_DNX_ITMH_DEST_TYPE_MULTICAST, /* ITMH destination type is multicast. */
    bcmPktDnxItmhDestTypeFlow = _SHR_PKT_DNX_ITMH_DEST_TYPE_FLOW, /* ITMH destination type is flow. */
    bcmPktDnxItmhDestTypeIngressShapingFlow = _SHR_PKT_DNX_ITMH_DEST_TYPE_INGRESS_SHAPING_FLOW, /* ITMH destination type is ingress
                                           shaping flow. */
    bcmPktDnxItmhDestTypeVport = _SHR_PKT_DNX_ITMH_DEST_TYPE_VPORT, /* ITMH destination type is out lif. */
    bcmPktDnxItmhDestTypeSystemPort = _SHR_PKT_DNX_ITMH_DEST_TYPE_SYSTEM_PORT /* ITMH destination type is system port. */
} bcm_pkt_dnx_itmh_dest_type_t;

/* Itmh destination. */
typedef struct bcm_pkt_dnx_itmh_dest_s {
    bcm_pkt_dnx_itmh_dest_type_t dest_type; /* Destination type */
    bcm_pkt_dnx_itmh_dest_type_t dest_extension_type; /* Destination Extension type */
    bcm_gport_t destination;            /* Destination Gport */
    bcm_multicast_t multicast_id;       /* Destination multicast */
    bcm_gport_t destination_ext;        /* Destination-Extension Gport */
} bcm_pkt_dnx_itmh_dest_t;

/* Itmh */
typedef struct bcm_pkt_dnx_itmh_s {
    uint8 inbound_mirror_disable;   /* If set, disable inbound mirroring
                                       (ITMH.IN_MIRR_DISABLE) */
    uint32 snoop_cmnd;              /* snoop command (ITMH.SNOOP_CMD) */
    uint32 prio;                    /* Traffic Class (ITMH.FWD_TRAFFIC_CLASS) */
    bcm_color_t color;              /* Color (aka Drop precedence, ITMH.FWD_DP) */
    bcm_pkt_dnx_itmh_dest_t dest;   /* Destination information */
} bcm_pkt_dnx_itmh_t;

/* ftmh action type. */
typedef enum bcm_pkt_dnx_ftmh_action_type_e {
    bcmPktDnxFtmhActionTypeForward = _SHR_PKT_DNX_FTMH_ACTION_TYPE_FORWARD, /* TM action is forward */
    bcmPktDnxFtmhActionTypeSnoop = _SHR_PKT_DNX_FTMH_ACTION_TYPE_SNOOP, /* TM action is snoop */
    bcmPktDnxFtmhActionTypeInboundMirror = _SHR_PKT_DNX_FTMH_ACTION_TYPE_INBOUND_MIRROR, /* TM action is inbound mirror. */
    bcmPktDnxFtmhActionTypeOutboundMirror = _SHR_PKT_DNX_FTMH_ACTION_TYPE_OUTBOUND_MIRROR, /* TM action is outbound mirror. */
    bcmPktDnxFtmhActionTypeMirror = _SHR_PKT_DNX_FTMH_ACTION_TYPE_MIRROR, /* TM action is mirror. */
    bcmPktDnxFtmhActionTypeStatisticalSampling = _SHR_PKT_DNX_FTMH_ACTION_TYPE_STATISTICAL_SAMPLING /* TM action is statistical sampling. */
} bcm_pkt_dnx_ftmh_action_type_t;

/* ftmh lb extension. */
typedef struct bcm_pkt_dnx_ftmh_lb_extension_s {
    uint8 valid;    /* Set if the extension is present */
    uint32 lb_key;  /* Load Balancing Key (FTMH.LB-Key) */
} bcm_pkt_dnx_ftmh_lb_extension_t;

/* ftmh dest extension. */
typedef struct bcm_pkt_dnx_ftmh_dest_extension_s {
    uint8 valid;                /* Set if the extension is present */
    bcm_gport_t dst_sysport;    /* Destination System Port
                                   (FTMH.Destination-Sys-Port)) */
} bcm_pkt_dnx_ftmh_dest_extension_t;

/* ftmh stack extension. */
typedef struct bcm_pkt_dnx_ftmh_stack_extension_s {
    uint8 valid;                    /* Set if the extension is present */
    uint32 stack_route_history_bmp; /* Route bitmap to prevent loops in stacking
                                       system
                                       (FTMH.Stacking-Route-History-Bitmap) */
} bcm_pkt_dnx_ftmh_stack_extension_t;

/* ASE type. */
typedef enum bcm_pkt_dnx_ase_type_e {
    bcmPktDnxAseTypeNone = 0,           /* Ase type is None */
    bcmPktDnxAseTypeTrajectoryTrace = 1, /* Ase type is Trajectory trace */
    bcmPktDnxAseTypeErspan = 2,         /* Ase type is ERSPAN */
    bcmPktDnxAseTypeSFlow = 3,          /* Ase type is sFlow */
    bcmPktDnxAseTypeMirrorOnDrop = 4,   /* Ase type is Mirror-On-Drop */
    bcmPktDnxAseTypeInt = 5             /* Ase type is Inband Network Telemetry */
} bcm_pkt_dnx_ase_type_t;

/* ERSPAN header information */
typedef struct bcm_pkt_dnx_erspan_info_s {
    uint8 direction;        /* Direction: 0x0-incoming, 0x1-outgoing */
    uint8 truncated_flag;   /* Truncated field in ERSPAN header. 0x1-it means
                               the frame copy encapsulated in the ERSPAN packet
                               will be truncated; 0x0-not truncated */
    uint8 en;               /* The trunk encapsulation type */
    uint8 cos;              /* Class of service */
    uint16 vlan;            /* Vlan */
} bcm_pkt_dnx_erspan_info_t;

/* Ase information */
typedef struct bcm_pkt_dnx_ase_info_s {
    bcm_pkt_dnx_erspan_info_t erspan_info; /* ERSPAN header information */
    uint8 int_profile;                  /* Inband Network Telemetry profile */
} bcm_pkt_dnx_ase_info_t;

/* ftmh application specific extension. */
typedef struct bcm_pkt_dnx_ftmh_ase_extension_s {
    uint8 valid;                        /* Set if the extension is present */
    bcm_pkt_dnx_ase_type_t ase_type;    /* Application Specific Extension type */
    bcm_pkt_dnx_ase_info_t ase_info;    /* Application Specific Extension
                                           information */
} bcm_pkt_dnx_ftmh_ase_extension_t;

/* ftmh flow_id extension. */
typedef struct bcm_pkt_dnx_ftmh_flow_id_extension_s {
    uint8 valid;        /* Set if the extension is present */
    uint32 flow_id;     /* Flow-ID */
    uint8 flow_profile; /* Flow-Profile */
} bcm_pkt_dnx_ftmh_flow_id_extension_t;

/* ftmh */
typedef struct bcm_pkt_dnx_ftmh_s {
    uint32 packet_size;                 /* Packet size in bytes
                                           (FTMH.Packet-Size) */
    uint32 prio;                        /* Traffic class (FTMH.Traffic-Class) */
    bcm_gport_t src_sysport;            /* Source System port
                                           (FTMH.Source-System-Port-Aggr) */
    bcm_gport_t dst_port;               /* Destination local port (FTMH.PP_DSP) */
    bcm_color_t ftmh_dp;                /* Drop precedence (FTMH.DP) */
    bcm_pkt_dnx_ftmh_action_type_t action_type; /* Action type (FTMH.TM-Action-Type) */
    uint8 out_mirror_disable;           /* Disable Outbound mirroring
                                           (FTMH.Out-Mirror-Disable) */
    uint8 is_mc_traffic;                /* Indicate if the traffic is multicast
                                           (FTMH.TM-Action-Is-Multicast) */
    bcm_multicast_t multicast_id;       /* Multicast ID (FTMH.Multicast-ID).
                                           Valid only if is_mc_traffic is set */
    bcm_gport_t out_vport;              /* Virtual port (FTMH.Out-LIF). Valid
                                           only if is_mc_traffic is unset */
    uint32 cni;                         /* Congestion indication (FTMH.CNI) */
    bcm_pkt_dnx_ftmh_lb_extension_t lb_ext; /* FTMH Load Balancing Key extension */
    bcm_pkt_dnx_ftmh_dest_extension_t dest_ext; /* FTMH Destination System Port
                                           Extension */
    bcm_pkt_dnx_ftmh_stack_extension_t stack_ext; /* FTMH Stacking extension
                                           (Stacking-Route-History-Bitmap) */
    bcm_pkt_dnx_ftmh_ase_extension_t ase_ext; /* FTMH Application Specific Extension */
    bcm_pkt_dnx_ftmh_flow_id_extension_t flow_id_ext; /* FTMH FLow-ID Extension */
} bcm_pkt_dnx_ftmh_t;

/* TSH header */
typedef struct bcm_pkt_dnx_tsh_s {
    uint8 valid;    /* Set if TSH is present */
} bcm_pkt_dnx_tsh_t;

/* otsh type */
typedef enum bcm_pkt_dnx_otsh_type_e {
    bcmPktDnxOtshTypeOam = _SHR_PKT_DNX_OTSH_TYPE_OAM, /* OAM-TS type is OAM */
    bcmPktDnxOtshType1588v2 = _SHR_PKT_DNX_OTSH_TYPE_1588v2, /* OAM-TS type is 1588v2 */
    bcmPktDnxOtshTypeReserved = _SHR_PKT_DNX_OTSH_TYPE_RESERVED, /* OAM-TS type is Reserved */
    bcmPktDnxOtshTypeLatency = _SHR_PKT_DNX_OTSH_TYPE_LATENCY /* OAM-TS type is packet latency */
} bcm_pkt_dnx_otsh_type_t;

/* otsh oam sutype */
typedef enum bcm_pkt_dnx_otsh_oam_subtype_e {
    bcmPktDnxOtshOamSubtypeNone = _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_NONE, /* None */
    bcmPktDnxOtshOamSubtypeLm = _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_LM, /* Loss Measurement (LM) */
    bcmPktDnxOtshOamSubtypeDm1588 = _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_DM1588, /* Delay Measurement (DM) - 1588 ToD */
    bcmPktDnxOtshOamSubtypeDmNtp = _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_DM_NTP, /* Delay Measurement (DM) - NTP ToD */
    bcmPktDnxOtshOamSubtypeOamDefault = _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_OAM_DEFAULT, /* Default OAM type */
    bcmPktDnxOtshOamSubtypeLoopback = _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_LOOPBACK, /* Loopback */
    bcmPktDnxOtshOamSubtypeEcn = _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_ECN /* ECN */
} bcm_pkt_dnx_otsh_oam_subtype_t;

/* otsh */
typedef struct bcm_pkt_dnx_otsh_s {
    bcm_pkt_dnx_otsh_type_t otsh_type;  /* OAM-TS(Type). */
    bcm_pkt_dnx_otsh_oam_subtype_t oam_sub_type; /* OAM-TS(OAM-Sub-Type). Applies only
                                           when otsh_type is oam. */
    uint32 oam_up_mep;                  /* OAM-TS(MEP-Type). Applies only when
                                           otsh_type is oam. */
    uint32 tp_cmd;                      /* OAM-TS(TP-Cmd). Applies only when
                                           otsh_type is 1588. */
    uint8 ts_encap;                     /* OAM-TS(TS-Encaps). Applies only when
                                           otsh_type is 1588. */
    uint64 oam_ts_data;                 /* OAM-TS(OAM-TS-Data) */
    uint32 latency_flow_ID;             /* Latency flow ID generated by the PMF. */
    uint32 offset;                      /* OAM-TS(offset). Applies only when
                                           otsh_type is oam.. */
} bcm_pkt_dnx_otsh_t;

/* otmh src sysport extension */
typedef struct bcm_pkt_dnx_otmh_src_sysport_extension_s {
    uint8 valid;                /* Set if the extension is present */
    bcm_gport_t src_sysport;    /* Source System Port (OTMH.Source-System-Port) */
} bcm_pkt_dnx_otmh_src_sysport_extension_t;

/* otmh vport extension */
typedef struct bcm_pkt_dnx_otmh_vport_extension_s {
    uint8 valid;            /* Set if the extension is present */
    bcm_gport_t out_vport;  /* Virtual port (OTMH.Out-LIF/CUD) */
} bcm_pkt_dnx_otmh_vport_extension_t;

/* otmh */
typedef struct bcm_pkt_dnx_otmh_s {
    bcm_pkt_dnx_ftmh_action_type_t action_type; /* Action type (OTMH.TM-Action-Type) */
    bcm_color_t ftmh_dp;                /* Drop precedence (OTMH.DP) */
    uint8 is_mc_traffic;                /* AIndicate if the traffic is multicast
                                           (OTMH.System-Multicast) */
    uint32 prio;                        /* Traffic class (OTMH.Traffic-Class) */
    bcm_gport_t dst_port;               /* Destination local port
                                           (OTMH.Destination-Port) */
    bcm_pkt_dnx_otmh_src_sysport_extension_t src_sysport_ext; /* OTMH Source System Port Extension */
    bcm_pkt_dnx_otmh_vport_extension_t out_vport_ext; /* OTMH Source System Port Extension */
} bcm_pkt_dnx_otmh_t;

/* Internals */
typedef struct bcm_pkt_dnx_internal_s {
    uint32 forward_domain;  /* VSI for bridging, VRF for routing, VFT for FCoE,
                               etc */
    uint32 trap_qualifier;  /* Trap Qualifier */
    uint32 trap_id;         /* Trap Id */
} bcm_pkt_dnx_internal_t;

/* UDH header */
typedef struct bcm_pkt_dnx_udh_s {
    uint8 size;     /* UDH size: 0/1/2/4 */
    uint32 data;    /* UDH data */
} bcm_pkt_dnx_udh_t;

/* dnx packet */
typedef struct bcm_pkt_dnx_s {
    bcm_pkt_dnx_type_t type;            /* DNX Header type */
    bcm_pkt_dnx_itmh_t itmh;            /* ITMH Header */
    bcm_pkt_dnx_ftmh_t ftmh;            /* FTMH Header */
    bcm_pkt_dnx_otsh_t otsh;            /* OAM-TS Header (OTSH) */
    bcm_pkt_dnx_otmh_t otmh;            /* OTMH Header */
    bcm_pkt_dnx_internal_t internal;    /* Internal Header */
} bcm_pkt_dnx_t;

/* Initialize a BCM packet structure. */
struct bcm_pkt_s { 
    bcm_pkt_blk_t *pkt_data;            /* Pointer to array of data blocks. */
    uint8 blk_count;                    /* Number of blocks in data array. */
    uint8 unit;                         /* Unit number. */
    uint8 cos;                          /* The local COS queue to use. */
    uint8 prio_int;                     /* Internal priority of the packet. */
    bcm_vlan_t vlan;                    /* 802.1q VID or VSI or VPN. */
    uint8 vlan_pri;                     /* Vlan tag priority . */
    uint8 vlan_cfi;                     /* Vlan tag CFI bit. */
    bcm_vlan_t inner_vlan;              /* Inner VID or VSI or VPN. */
    uint8 inner_vlan_pri;               /* Inner vlan tag priority . */
    uint8 inner_vlan_cfi;               /* Inner vlan tag CFI bit. */
    bcm_color_t color;                  /* Packet color. */
    int16 src_port;                     /* Source port used in header/tag. */
    bcm_trunk_t src_trunk;              /* Source trunk group ID used in
                                           header/tag, -1 if src_port set . */
    uint16 src_mod;                     /* Source module ID used in header/tag. */
    uint16 dest_port;                   /* Destination port used in header/tag. */
    uint16 dest_mod;                    /* Destination module ID used in
                                           header/tag. */
    uint8 opcode;                       /* BCM_HG_OPCODE_xxx. */
    bcm_gport_t dst_gport;              /* Destination virtual port */
    bcm_gport_t src_gport;              /* Source virtual port */
    bcm_multicast_t multicast_group;    /* Destination multicast group. */
    uint32 stk_flags;                   /* Stacking header flags. */
    bcm_pkt_stk_forward_t stk_forward;  /* Stacking header forwarding opcode. */
    uint32 stk_classification_tag;      /* Stacking header classification tag. */
    uint32 stk_pkt_prio;                /* Stacking header new packet priority. */
    uint32 stk_dscp;                    /* Stacking header new DSCP. */
    uint32 stk_load_balancing_number;   /* Stacking header load balancing
                                           number. */
    bcm_if_t stk_encap_id;              /* Stacking header encapsulation ID for
                                           remote packet replication. */
    uint16 pkt_len;                     /* Packet length according to flags. */
    uint16 tot_len;                     /* Packet length as transmitted or
                                           received. */
    bcm_pbmp_t tx_pbmp;                 /* Target ports. */
    bcm_pbmp_t tx_upbmp;                /* Untagged target ports. */
    bcm_pbmp_t tx_l3pbmp;               /* L3 ports. */
    bcm_port_t pkt_trace_src_port;      /* pkt_trace_src_port */
    uint8 pfm;                          /* BCM_PORT_PFM_xxx flags. */
    uint32 rx_reason;                   /* Opcode from packet. */
    bcm_rx_reasons_t rx_reasons;        /* Set of packet "reasons". */
    uint32 rx_path;                     /* Rx path of packet. */
    uint8 rx_unit;                      /* Local rx unit. */
    uint8 rx_port;                      /* Local rx port; not in HG hdr. */
    uint8 rx_cpu_cos;                   /* CPU may get pkt on diff cos. */
    uint8 rx_untagged;                  /* The packet was untagged on ingress. */
    uint32 rx_classification_tag;       /* Classification tag. */
    uint32 rx_matched;                  /* Field processor matched rule. */
    bcm_if_t rx_l3_intf;                /* L3 egress object interface ID. */
    bcm_vlan_action_t rx_outer_tag_action; /* Outer-tag action applied to packet. */
    bcm_vlan_action_t rx_inner_tag_action; /* Inner-tag action applied to packet. */
    uint32 rx_timestamp;                /* Time stamp of time sync protocol
                                           packets. */
    uint32 rx_timestamp_upper;          /* Upper 32-bit of 64-bit timestamp of
                                           OAM DM. */
    uint32 timestamp_flags;             /* Timestamp flags. */
    void *cookie;                       /* User data for callback. */
    void *cookie2;                      /* Additional user data for callback. */
    bcm_pkt_cb_f call_back;             /* Callback function. */
    uint32 flags;                       /* BCM_PKT_F_xxx flags. */
    void *next;                         /* When linked into lists. */
    int8 dma_channel;                   /* DMA channel used; may be -1. */
    bcm_pkt_blk_t _pkt_data;            /* For single block packets (internal). */
    bcm_pkt_t *_last_pkt;               /* To link to end of linked lists
                                           (internal). */
    void *_dv;                          /* DV controlling this packet
                                           (internal). */
    int8 _idx;                          /* Packet's index in the DV for RX
                                           (internal). */
    bcm_pkt_t *_next;                   /* For BCM layer linked lists
                                           (internal). */
    void *alloc_ptr;                    /* Pointer for deallocation (internal). */
    void *trans_ptr;                    /* Transport pointer associated with
                                           packet (internal). */
    uint8 _higig[16];                   /* HiGig header value (network byte
                                           order). */
    uint8 _pb_hdr[12];                  /* Pipe Bypass Header (network byte
                                           order). */
    uint8 _sltag[4];                    /* SL tag value (network byte order). */
    uint8 _vtag[4];                     /* VLAN tag if not in packet (network
                                           byte order). */
    uint8 _dpp_hdr[10];                 /* DPP header contents */
    int _dpp_hdr_type;                  /* DPP header and extensions type */
    int flow_id;                        /* Internal flow id. */
    uint32 filter_enable;               /* filters to be enabled. */
    bcm_pbmp_t _dv_tx_pbmp;             /* Actual pbmp assigned to tx
                                           descriptor(internal). */
    bcm_pbmp_t _dv_tx_upbmp;            /* Actual upbmp assigned to tx
                                           descriptor(internal). */
    uint32 flags2;                      /* BCM_PKT_F2_xxx flags. */
    uint8 oam_replacement_type;         /* OAM replacement type used in SOBMH
                                           header. */
    uint8 oam_replacement_offset;       /* OAM replacement offset used in SOBMH
                                           header. */
    uint16 oam_lm_counter_index;        /* OAM LM counter index used in SOBMH
                                           header. */
    uint32 rx_trap_data;                /* Additional trap information */
    void *_dcb;                         /* Pointer for dcb. */
    uint16 oam_lm_counter_index_2;      /* OAM Second LM counter index used in
                                           SOBMH header. */
    uint16 ma_ptr;                      /* OAM MA Pointer value. For BCM5645x,
                                           this corresponds to endpoint group
                                           index */
    bcm_pkt_timestamp_mode_t timestamp_mode; /* OAM DM timestamp mode */
    bcm_pkt_oam_lm_counter_mode_t counter_mode_1; /* OAM LM counter-1 mode */
    bcm_pkt_oam_lm_counter_mode_t counter_mode_2; /* OAM LM counter-2 mode */
    uint8 timestamp_offset;             /* Offset to place the timestamp in the
                                           packet. */
    bcm_rx_decap_tunnel_t rx_decap_tunnel; /* Tunnel Decap during packet rx */
    bcm_gport_t src_vport;              /* Source VPort (In-LIF) */
    bcm_gport_t dst_vport;              /* Destination VPort (Out-LIF) */
    uint32 fwd_hdr_offset;              /* Distance (in bytes) to forwarding
                                           header from start of packet */
    int snoop_cmnd;                     /* Snoop command */
    bcm_gport_t stk_dst_gport;          /* Stacking destination port */
    uint32 stk_route_tm_domains;        /* Bitmap of the traversed TM domains */
    uint32 oam_hdr_offset;              /* Distance (in bytes) to OAM header
                                           from start of packet */
    uint8 oam_lm_replacement_offset;    /* Replacement offset for LM counter in
                                           Bytes */
    bcm_pkt_rx_oam_type_t rx_oam_pkt_type; /* OAM Pkt Type. */
    bcm_pkt_oam_counter_t oam_counter[BCM_PKT_OAM_COUNTER_MAX]; /* OAM counter array. */
    uint32 oam_counter_size;            /* Size of Counter array */
    uint8 _olp_hdr[20];                 /* OLP Header (network byte order). */
    bcm_pkt_dnx_t dnx_header_stack[BCM_PKT_NOF_DNX_HEADERS]; /* DNX Header stack */
    uint8 dnx_header_count;             /* Number of DNX headers */
    uint8 _ext_higig[4];                /* Extended HiGig Header (network byte
                                           order). */
    void *tx_header;                    /* Pointer for Packet tx header
                                           (internal). */
    uint8 txprocmh_qos_fields_valid;    /* QoS properties to be picked up from
                                           packet */
    uint8 txprocmh_congestion_int;      /* Congestion priority of the packet. */
    uint8 txprocmh_mcast_lb_index_valid; /* If set, use the value from
                                           txprocmh_mcast_lb_index */
    uint8 txprocmh_mcast_lb_index;      /* Software to select load balancing
                                           bitmap */
    uint16 txprocmh_ecmp_group_index;   /* Single level ECMP group index */
    uint16 txprocmh_ecmp_member_index;  /* ECMP member table index used by a
                                           single level ECMP group */
    uint16 txprocmh_destination;        /* Destination to be used based on
                                           txprocmh_destination_type */
    uint8 txprocmh_destination_type;    /* Destination Type/opcode of the packet */
    uint16 egress_to_cpu_hdr_size;      /* Size of egress to CPU header in CMIC
                                           devices. This header is applicable
                                           only in the new generation of CMIC
                                           called CMIC-X. */
    uint32 match_id[2];                 /* Match ID information. */
    uint8 rqe_q_num;                    /* RQE Queue Number */
    uint32 forwarding_type;             /* Packet forwarding type */
    uint32 forwarding_zone_id;          /* Indicates packet was routed - UC OR
                                           MC */
    uint8 spid;                         /* Service Pool ID (size 2 bits) */
    uint8 spid_override;                /* Indicates SPID to be used from Tx
                                           header */
    uint32 replication_or_nhi;          /* Replication ID or Next hop index */
};

/* HiGig opcodes. */
#define BCM_HG_OPCODE_CPU       0x00       /* CPU Frame. */
#define BCM_HG_OPCODE_UC        0x01       /* Unicast Frame. */
#define BCM_HG_OPCODE_BC        0x02       /* Broadcast or DLF frame. */
#define BCM_HG_OPCODE_MC        0x03       /* Multicast Frame. */
#define BCM_HG_OPCODE_IPMC      0x04       /* IP Multicast Frame. */

/* Generic packet opcodes. */
#define BCM_PKT_OPCODE_CPU      0x00       /* CPU Frame. */
#define BCM_PKT_OPCODE_UC       0x01       /* Unicast Frame. */
#define BCM_PKT_OPCODE_BC       0x02       /* Broadcast or DLF frame. */
#define BCM_PKT_OPCODE_MC       0x03       /* Multicast Frame. */
#define BCM_PKT_OPCODE_IPMC     0x04       /* IP Multicast Frame. */

/* Packet flags. */
#define BCM_PKT_F_HGHDR             0x1        /* HiGig header is active
                                                  (internal). */
#define BCM_PKT_F_SLTAG             0x2        /* SL tag is active. */
#define BCM_PKT_F_NO_VTAG           0x4        /* Packet does not contain VLAN
                                                  tag. */
#define BCM_PKT_F_TX_UNTAG          0x8        /* TX packet untagged (internal). */
#define BCM_TX_CRC_FLD              0xf0       /* CRC information */
#define BCM_TX_CRC_ALLOC            0x10       /* Allocate buffer for CRC. */
#define BCM_TX_CRC_REGEN            0x20       /* Regenerate CRC. */
#define BCM_TX_CRC_APPEND           (BCM_TX_CRC_ALLOC + BCM_TX_CRC_REGEN) 
#define BCM_TX_CRC_FORCE_ERROR      0x40       /* Force CRC error. */
#define BCM_TX_NO_PAD               0x100      /* Do not pad runt packets. */
#define BCM_TX_FAST_PATH            0x200      /* Fast path TX. */
#define BCM_TX_PURGE                0x400      /* XGS3 Set PURGE bit in DCB
                                                  (internal). */
#define BCM_TX_LINKDOWN_TRANSMIT    0x800      /* Transmit on link down ports */
#define BCM_TX_RELIABLE             0x1000     /* Relay (tunnel) packet
                                                  reliably. */
#define BCM_TX_BEST_EFFORT          0x2000     /* Use best effort to relay
                                                  packet. */
#define BCM_TX_LOOPBACK             0x4000     /* Loopback is indicated. */
#define BCM_TX_PKT_PROP_ANY         0xf0000    /* All packet property fields. */
#define BCM_TX_SRC_MOD              0x10000    /* Use the src_mod field from
                                                  packet. */
#define BCM_TX_SRC_PORT             0x20000    /* Use the src_port field from
                                                  packet. */
#define BCM_TX_PRIO_INT             0x40000    /* Use the prio_int field from
                                                  packet. */
#define BCM_TX_PFM                  0x80000    /* Use PFM field from packet. */
#define BCM_TX_ETHER                0x100000   /* Fully mapped packet TX. */
#define BCM_TX_HG_READY             0x200000   /* HiGig header in _higig. */
#define BCM_TX_TIME_STAMP_REPORT    0x400000   /* Request transmit time stamp. */
#define BCM_RX_LEARN_DISABLED       0x800000   /* Packet's SA is not learned. */
#define BCM_RX_CRC_STRIP            0x1000000  /* Do not include the CRC in the
                                                  length of the packet. */
#define BCM_RX_TUNNELLED            0x2000000  /* Packet was tunnelled. */
#define BCM_RX_MIRRORED             0x4000000  /* Packet was mirrored. */
#define BCM_RX_TRUNCATED            0x8000000  /* Packet was truncated. */
#define BCM_PKT_F_TIMESYNC          0x10000000 /* Packet is for Time Sync
                                                  protocol. */
#define BCM_PKT_F_TRUNK             0x20000000 /* Trunk port. */
#define BCM_PKT_F_TEST              0x40000000 /* Set the Test bit. */
#define BCM_PKT_F_ROUTED            0x80000000 /* L3 switched packet. */
#define BCM_TX_EXT_HG_HDR           0x20       /* Extended HiGig Header. */
#define BCM_TX_NO_VISIBILITY_RESUME 0x8000     /* Disable Visibility resume for
                                                  DNX. */

/* Packet flags2. */
#define BCM_PKT_F2_REPLACEMENT_TYPE         0x1        /* OAM replacement type. */
#define BCM_PKT_F2_REPLACEMENT_OFFSET       0x2        /* OAM replacement
                                                          offset. */
#define BCM_PKT_F2_LM_COUNTER_INDEX         0x4        /* OAM LM counter index. */
#define BCM_PKT_F2_TIMESTAMP_MODE           0x8        /* OAM DM time stamp type
                                                          - see
                                                          bcm_pkt_timestamp_mode_t
                                                          element, No-op by
                                                          default */
#define BCM_PKT_F2_SAMPLE_RDI               0x10       /* OAM - Sample RDI bit. */
#define BCM_PKT_F2_MA_PTR                   0x20       /* OAM - MA Pointer . */
#define BCM_PKT_F2_MEP_TYPE_UPMEP           0x40       /* OAM - MEP type -
                                                          UP/DOWN */
#define BCM_PKT_F2_LM_COUNTER_INDEX_2       0x80       /* OAM -Second LM counter
                                                          index. */
#define BCM_PKT_F2_COUNTER_MODE_1           0x100      /* OAM - Counter 1 mode -
                                                          Use specified counter
                                                          actions, no-op by
                                                          default */
#define BCM_PKT_F2_COUNTER_MODE_2           0x200      /* OAM - Counter 2 mode -
                                                          Use specified counter
                                                          actions, no-op by
                                                          default */
#define BCM_PKT_F2_SNOOPED                  0x400      /* Packet was snooped */
#define BCM_PKT_F2_UNKNOWN_DEST             0x800      /* packet has unknown
                                                          destination */
#define BCM_PKT_F2_RX_PORT                  0x1000     /* indicate that
                                                          bcm_pkt_s.rx_port is
                                                          used as a source port
                                                          for the
                                                          masquerade/visibility
                                                          feature */
#define BCM_PKT_F2_VISIBILITY_PKT           0x2000     /* indication that this
                                                          is a visibility
                                                          packet(internal) */
#define BCM_PKT_F2_OAM_TX                   0x4000     /* Indicates OAM Packet
                                                          Tx */
#define BCM_PKT_F2_OLP_READY                0x8000     /* Indicates OLP already
                                                          constructed in field
                                                          _olp_hdr */
#define BCM_PKT_F2_LM_REPLACEMENT_OFFSET    0x10000    /* OAM LM replacement
                                                          offset */
#define BCM_PKT_F2_MEP_TYPE_SAT_DOWNMEP     0x20000    /* SAT MEP TYPE - DOWN */
#define BCM_PKT_F2_MEP_TYPE_SAT_UPMEP       0x40000    /* SAT MEP TYPE - UP */
#define BCM_PKT_F2_MC_QUEUE                 0x80000    /* Indicates packet goes
                                                          to MC queue */
#define BCM_PKT_F2_CPU_TX_PROC              0x100000   /* Indicates CPU TX PROC
                                                          packet */
#define BCM_PKT_F2_CONG_INT                 0x200000   /* use the
                                                          txprocmh_congestion_int
                                                          field from packet */
#define BCM_PKT_F2_EXT_HG_HDR               0x400000   /* use the extended higig
                                                          field from packet */
#define BCM_PKT_F2_TWAMP_OWAMP_TS           0x800000   /* Requires
                                                          BROADCOM_PREMIUM
                                                          license */
#define BCM_PKT_F2_SPID_OVERRIDE            0x1000000  /* Use SPID from the
                                                          packet */

/* Flags for rx_untagged field in bcm_pkt_t structure. */
#define BCM_PKT_OUTER_UNTAGGED  0x1        /* Packet received without outer vlan
                                              tag. */
#define BCM_PKT_INNER_UNTAGGED  0x2        /* Packet received without inner vlan
                                              tag. */

/* TimeSync Packet Flags. */
#define BCM_TX_TIMESYNC_ONE_STEP            0x1        /* One step timestamp. */
#define BCM_TX_TIMESYNC_ONE_STEP_INGRESS_SIGN 0x2        /* Ingress timestamp sign
                                                          bit. */
#define BCM_TX_TIMESYNC_ONE_STEP_HDR_START_OFFSET 0x4        /* One step timestamp
                                                          header offset. */
#define BCM_TX_TIMESYNC_ONE_STEP_REGEN_UDP_CHKSUM 0x8        /* One step timestamp
                                                          header offset. */
#define BCM_TX_TIMESYNC_INGRESS_SIGN        (BCM_TX_TIMESYNC_ONE_STEP_INGRESS_SIGN) /* Ingress timestamp sign
                                                          bit. */
#define BCM_TX_TIMESYNC_HDR_START_OFFSET    (BCM_TX_TIMESYNC_ONE_STEP_HDR_START_OFFSET) /* PTP header offset in
                                                          packet buffer. */
#define BCM_TX_TIMESYNC_REGEN_UDP_CHKSUM    (BCM_TX_TIMESYNC_ONE_STEP_REGEN_UDP_CHKSUM) /* Regenerate UDP header
                                                          checksum of PTP
                                                          packet. */

/* Stacking Packet flags. */
#define BCM_PKT_STK_F_MIRROR                0x1        /* Mirror packet. */
#define BCM_PKT_STK_F_DO_NOT_MODIFY         0x2        /* Do not alter packet on
                                                          egress device. */
#define BCM_PKT_STK_F_TRUNK_FAILOVER        0x4        /* Packet is redirected
                                                          due to trunk failover. */
#define BCM_PKT_STK_F_SRC_PORT              0x8        /* Source GPORT provided. */
#define BCM_PKT_STK_F_DST_PORT              0x10       /* Destination GPORT
                                                          provided. */
#define BCM_PKT_STK_F_DEFERRED_DROP         0x20       /* Drop in egress device. */
#define BCM_PKT_STK_F_DEFERRED_CHANGE_PKT_PRIO 0x40       /* Update packet priority
                                                          to stk_pkt_prio in
                                                          egress device. */
#define BCM_PKT_STK_F_DEFERRED_CHANGE_DSCP  0x80       /* Update DSCP to
                                                          stk_dscp in egress
                                                          device. */
#define BCM_PKT_STK_F_CLASSIFICATION_TAG    0x100      /* stk_classification_tag
                                                          field is valid. */
#define BCM_PKT_STK_F_VLAN_TRANSLATE_NONE   0x200      /* No VLAN translation
                                                          performed. */
#define BCM_PKT_STK_F_VLAN_TRANSLATE_UNCHANGED 0x400      /* VLAN translation did
                                                          not change packet. */
#define BCM_PKT_STK_F_VLAN_TRANSLATE_CHANGED 0x800      /* VLAN translation
                                                          changed packet. */
#define BCM_PKT_STK_F_DO_NOT_LEARN          0x1000     /* Packet should not be
                                                          learned on egress
                                                          device. */
#define BCM_PKT_STK_F_PRESERVE_DSCP         0x2000     /* Egress device should
                                                          not change DSCP. */
#define BCM_PKT_STK_F_PRESERVE_PKT_PRIO     0x4000     /* Egress device should
                                                          not change packet
                                                          priority. */
#define BCM_PKT_STK_F_TX_TAG                0x8000     /* The VLAN tag should be
                                                          included in the packet
                                                          data (internal). */
#define BCM_PKT_STK_F_ENCAP_ID              0x10000    /* The stk_encap_id value
                                                          is provided. */
#define BCM_PKT_STK_F_FAILOVER              0x20000    /* Use the protection
                                                          nexthop instead of the
                                                          default nexthop. */

/* internal packet trace flags */
#define BCM_PKT_TRACE_LEARN                 0x1        /* learning ensable on
                                                          internal trace packet */
#define BCM_PKT_TRACE_NO_IFP                0x2        /* disable ingress field
                                                          processor lookup on
                                                          internal trace packet */
#define BCM_PKT_TRACE_FORWARD               0x4        /* forward internal trace
                                                          packet to egress */
#define BCM_PKT_TRACE_DROP_TO_MMU           0x8        /* Drop trace packet to
                                                          MMU else forward to
                                                          egress */
#define BCM_PKT_TRACE_DROP_TO_EGR           0x10       /* Drop trace packet to
                                                          egress */
#define BCM_PKT_TRACE_RETRIEVE_DATA_ONLY    0x20       /* Get the visibility
                                                          data with no manual
                                                          visibility packet
                                                          inject */

/* Filter types. */
#define BCM_PKT_FILTER_LAG          0x1        /* LAG filter. */
#define BCM_PKT_FILTER_TAGGED       0x2        /* Tag filter. */
#define BCM_PKT_FILTER_PORT_MASK    0x4        /* Port mask filter. */
#define BCM_PKT_FILTER_STP          0x8        /* STP filter. */
#define BCM_PKT_FILTER_EAP          0x10       /* EAP filter. */
#define BCM_PKT_FILTER_INGRESS_VLAN 0x20       /* Ingress Vlan filter. */
#define BCM_PKT_FILTER_EGRESS_VLAN  0x40       /* Egress Vlan filter. */
#define BCM_PKT_FILTER_SA           0x80       /* Source address filter. */

/* CPU to CPU tunnel modes. */
typedef enum bcm_cpu_tunnel_mode_e {
    BCM_CPU_TUNNEL_NONE = 0,            /* Do not tunnel. */
    BCM_CPU_TUNNEL_PACKET_RELIABLE = 1, /* Use reliable transport. */
    BCM_CPU_TUNNEL_PACKET_BEST_EFFORT = 2, /* Use best effort transport. */
    BCM_CPU_TUNNEL_PACKET = 3           /* Use default mode. */
} bcm_cpu_tunnel_mode_t;

/* CPU to CPU tunnel flags. */
#define BCM_CPU_TUNNEL_F_UNTAGGED       0x1        /* Untagged packet. */
#define BCM_CPU_TUNNEL_F_L3             0x2        /* L3 packet. */
#define BCM_CPU_TUNNEL_F_PBMP           0x4        /* Use port bitmap, not just
                                                      port. */
#define BCM_CPU_TUNNEL_F_RELIABLE       0x8        /* Use reliable transport. */
#define BCM_CPU_TUNNEL_F_BEST_EFFORT    0x10       /* Use best effort transport. */
#define BCM_CPU_TUNNEL_F_ALL_UNTAGGED   0x20       /* Use with port bitmap. */

/* Backward compatibility. */
#define bcm_tunnel_mode_t       bcm_cpu_tunnel_mode_t 

/* Backward compatibility. */
#define BCM_TUNNEL_NONE             BCM_CPU_TUNNEL_NONE 
#define BCM_TUNNEL_PACKET_RELIABLE  BCM_CPU_TUNNEL_PACKET_RELIABLE 
#define BCM_TUNNEL_PACKET_NO_ACK    BCM_CPU_TUNNEL_PACKET_BEST_EFFORT 
#define BCM_TUNNEL_PACKET           BCM_CPU_TUNNEL_PACKET 

/* Backward compatibility. */
#define BCM_TUNNEL_F_UNTAGGED       BCM_CPU_TUNNEL_F_UNTAGGED 
#define BCM_TUNNEL_F_L3             BCM_CPU_TUNNEL_F_L3 
#define BCM_TUNNEL_F_PBMP           BCM_CPU_TUNNEL_F_PBMP 
#define BCM_TUNNEL_F_RELIABLE       BCM_CPU_TUNNEL_F_RELIABLE 
#define BCM_TUNNEL_F_BEST_EFFORT    BCM_CPU_TUNNEL_F_BEST_EFFORT 
#define BCM_TUNNEL_F_ALL_UNTAGGED   BCM_CPU_TUNNEL_F_ALL_UNTAGGED 

/* Set up a single buffer packet. */
#define BCM_PKT_ONE_BUF_SETUP(pkt, buf, _len)  \
    do { \
        (pkt)->_pkt_data.data = (buf); \
        (pkt)->_pkt_data.len = (_len); \
        (pkt)->pkt_data = &(pkt)->_pkt_data; \
        (pkt)->blk_count = 1; \
    } while (0) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_TX_LEN_SET(pkt, _len)  \
    (pkt)->_pkt_data.len = (_len) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define _BCM_HTONS_CVT_SET(pkt, val, posn)  \
    do { \
         uint16 _tmp; \
         _tmp = bcm_htons(val); \
         sal_memcpy((pkt)->_pkt_data.data + (posn), &_tmp, 2); \
    } while (0) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_DMAC_SET(pkt, mac)  \
    sal_memcpy((pkt)->_pkt_data.data, (mac), 6) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_SMAC_SET(pkt, mac)  \
    sal_memcpy((pkt)->_pkt_data.data + 6, (mac), 6) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_TPID_SET(pkt, tpid)  \
    _BCM_HTONS_CVT_SET(pkt, tpid, 12) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_UNTAGGED_LEN_SET(pkt, len)  \
    _BCM_HTONS_CVT_SET(pkt, len, 12) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_VTAG_CONTROL_SET(pkt, vtag)  \
    _BCM_HTONS_CVT_SET(pkt, vtag, 14) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_TAGGED_LEN_SET(pkt, len)  \
    _BCM_HTONS_CVT_SET(pkt, len, 16) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_SNAP_DSAP_SET(pkt, dsap)  \
    (pkt)->_pkt_data.data[18] = (dsap) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_SNAP_SSAP_SET(pkt, ssap)  \
    (pkt)->_pkt_data.data[19] = (ssap) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_SNAP_CONTROL_SET(pkt, ctl)  \
    (pkt)->_pkt_data.data[20] = (ctl) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_SNAP_ORG_SET(pkt, b1, b2, b3)  \
     do { \
        (pkt)->_pkt_data.data[21] = (b1); \
        (pkt)->_pkt_data.data[22] = (b2); \
        (pkt)->_pkt_data.data[23] = (b3); \
    } while (0) 

/* Helper macro for single buffer Ethernet packet (not HiGig). */
#define BCM_PKT_HDR_SNAP_ETYPE_SET(pkt, etype)  \
    _BCM_HTONS_CVT_SET(pkt, etype, 24) 

/* Set a single port for a packet for TX. */
#define BCM_PKT_PORT_SET(pkt, _port, _untagged, _l3)  \
    do { \
        BCM_PBMP_PORT_SET((pkt)->tx_pbmp, _port); \
        if ((_untagged) != FALSE) { \
            BCM_PBMP_PORT_SET((pkt)->tx_upbmp, _port); \
        } else { \
            BCM_PBMP_CLEAR((pkt)->tx_upbmp); \
        } \
        if ((_l3) != FALSE) { \
            BCM_PBMP_PORT_SET((pkt)->tx_l3pbmp, _port); \
        } else { \
            BCM_PBMP_CLEAR((pkt)->tx_l3pbmp); \
        } \
    } while (0) 

/* Check if HiGig header is active in the packet. */
#define BCM_PKT_HAS_HGHDR(pkt)  \
    (((pkt)->flags & BCM_PKT_F_HGHDR) != 0) 

/* Check if SL tag is active in the packet. */
#define BCM_PKT_HAS_SLTAG(pkt)  \
    (((pkt)->flags & BCM_PKT_F_SLTAG) != 0) 

/* Check if VLAN tag is not present in packet. */
#define BCM_PKT_NO_VLAN_TAG(pkt)  \
    (((pkt)->flags & BCM_PKT_F_NO_VTAG) != 0) 

/* Do not include CRC in length on RX. */
#define BCM_PKT_RX_CRC_STRIP(pkt)  \
    ((pkt)->flags & BCM_RX_CRC_STRIP) 

/* Strip VLAN tag on RX; do not include in length. */
#define BCM_PKT_RX_VLAN_TAG_STRIP(pkt)  \
    BCM_PKT_NO_VLAN_TAG(pkt) 

#define BCM_TX_PKT_PROP_ANY_TST(pkt)  \
    (((pkt)->flags & BCM_TX_PKT_PROP_ANY) != 0) 

#define BCM_PKT_TX_ETHER(pkt)   \
    (((pkt)->flags & BCM_TX_ETHER) != 0) 

#define BCM_PKT_TX_HG_READY(pkt)  \
    (((pkt)->flags & BCM_TX_HG_READY) != 0) 

/* 
 * Check if fabric mapped, which means that the packet is sent
 * from the CPU through the HiGig ingress pipeline.
 */
#define BCM_PKT_TX_FABRIC_MAPPED(pkt)  \
            ((((pkt)->flags & BCM_TX_ETHER ) !=0 ) && \
             (((pkt)->flags & (BCM_TX_PKT_PROP_ANY | BCM_TX_HG_READY) ) !=0 )) 

/* Pointer to beginning of IEEE packet. */
#define BCM_PKT_IEEE(pkt)       \
    ((uint8*)((pkt)->pkt_data[0].data)) 

/* Length of IEEE packet including MAC addresses and CRC. */
#define BCM_PKT_IEEE_LEN(pkt)   \
    ((pkt)->pkt_len) 

/* Pointer to destination MAC address. */
#define BCM_PKT_DMAC(pkt)       \
    BCM_PKT_IEEE(pkt) 

/* Pointer to VLAN tag (all 4 bytes). */
#define BCM_PKT_VLAN_PTR(pkt)   \
   (((pkt)->flags & BCM_PKT_F_NO_VTAG) ? (pkt)->_vtag : \
    ((BCM_PKT_DMAC(pkt) + 2*sizeof(bcm_mac_t)))) 

/* The protocol (eg 0x8100) bytes. */
#define BCM_PKT_TAG_PROTOCOL(pkt)  \
    ((uint16) ((BCM_PKT_VLAN_PTR(pkt)[0] << 8) | (BCM_PKT_VLAN_PTR(pkt)[1]))) 

/* The VLAN control tag. */
#define BCM_PKT_VLAN_CONTROL(pkt)  \
    ((uint16) ((BCM_PKT_VLAN_PTR(pkt)[2] << 8) | (BCM_PKT_VLAN_PTR(pkt)[3]))) 

/* The ID from the VLAN control tag. */
#define BCM_PKT_VLAN_ID(pkt)    \
    BCM_VLAN_CTRL_ID(BCM_PKT_VLAN_CONTROL(pkt)) 

/* The priority from the VLAN control tag. */
#define BCM_PKT_VLAN_PRI(pkt)   \
    BCM_VLAN_CTRL_PRIO(BCM_PKT_VLAN_CONTROL(pkt)) 

/* The CFI from the VLAN control tag. */
#define BCM_PKT_VLAN_CFI(pkt)   \
     BCM_VLAN_CTRL_CFI(BCM_PKT_VLAN_CONTROL(pkt)) 

/* Pointer to packet's HiGig header. */
#define BCM_PKT_HG_HDR(pkt)     \
    ((pkt)->_higig) 

/* Pointer to packet's SL tag. */
#define BCM_PKT_SL_TAG(pkt)     \
    ((pkt)->_sltag) 

/* Pointer to packet's Pipeline Bypass header. */
#define BCM_PKT_PB_HDR(pkt)     \
    ((pkt)->_pb_hdr) 

/* Pointer to packet's Extended HiGig header. */
#define BCM_PKT_EXT_HG_HDR(pkt)  \
    ((pkt)->_ext_higig) 

#define BCM_PKT_NO_VTAG_REQUIRE(pkt)  \
    ((pkt)->flags |= BCM_PKT_F_NO_VTAG) 

#define BCM_PKT_HGHDR_CLR(pkt)  \
    ((pkt)->flags &= (~BCM_PKT_F_HGHDR)) 

#define BCM_PKT_HGHDR_REQUIRE(pkt)  \
    ((pkt)->flags |= BCM_PKT_F_HGHDR) 

#define BCM_PKT_SLTAG_REQUIRE(pkt)  \
    ((pkt)->flags |= BCM_PKT_F_SLTAG) 

/* How many bytes in the data blocks. */
#define BCM_PKT_BLK_BYTES_CALC(pkt, bytes)  \
    do { \
        int i; \
        (bytes) = 0; \
        for (i = 0; i < (pkt)->blk_count; i++) { \
            (bytes) += (pkt)->pkt_data[i].len; \
        } \
    } while (0) 

/* Packet length calculated based on flags. */
#define BCM_PKT_TX_LEN(pkt, bytes)  \
    do { \
        BCM_PKT_BLK_BYTES_CALC(pkt, bytes); \
        if (BCM_PKT_HAS_HGHDR(pkt)) (bytes) += 12; \
        if (BCM_PKT_HAS_SLTAG(pkt)) (bytes) += 4; \
        if ((pkt)->flags & BCM_PKT_F_NO_VTAG) (bytes) += 4; \
    } while (0) 

/* Packet length including SL, HiGig, VLAN tags. */
#define BCM_PKT_RX_TOT_LEN(pkt, bytes)  \
    (bytes) = (pkt)->tot_len 

/* 
 * BCM packet allocation function.
 * 
 * If unit is specified, the flags should be set up according to the unit
 * as best as possible.
 * 
 * If len > 0, then a packet buffer should be allocated and the packet
 * will be setup up for a single data block. If len <= 0, no packet
 * allocation will be made.
 */
typedef int (*bcm_pkt_alloc_f)(
    int unit, 
    int len, 
    bcm_pkt_t **pkt_buf);

/* BCM packet free function. */
typedef int (*bcm_pkt_free_f)(
    int unit, 
    bcm_pkt_t *pkt);

/* Map target ports according to VLAN and L2 tables. */
typedef int (*bcm_pkt_l2_map_f)(
    int unit, 
    bcm_pkt_t *pkt, 
    bcm_mac_t dest_mac, 
    int vid);

/* Initialize and set up a bcm_pkt_t structure. */
extern int bcm_pkt_clear(
    int unit, 
    bcm_pkt_t *pkt, 
    bcm_pkt_blk_t *blks, 
    int blk_count, 
    uint32 flags, 
    bcm_pkt_t **pkt_buf);

/* Initialize packet flags based on the type of device. */
extern int bcm_pkt_flags_init(
    int unit, 
    bcm_pkt_t *pkt, 
    uint32 init_flags);

/* Set up the length and flags for a bcm_pkt_t structure. */
extern int bcm_pkt_flags_len_setup(
    int unit, 
    bcm_pkt_t *pkt, 
    int alloc_bytes, 
    int payload_len, 
    uint32 flags);

/* Copy data into the data blocks of a packet structure. */
extern int bcm_pkt_memcpy(
    bcm_pkt_t *pkt, 
    int dest_byte, 
    uint8 *src, 
    int len);

/* Calculate a byte offset in the data buffer for a packet. */
extern int bcm_pkt_byte_index(
    bcm_pkt_t *pkt, 
    int n, 
    int *len, 
    int *blk_idx, 
    uint8 **location);

/* Allocate a packet structure and packet data. */
extern int bcm_pkt_alloc(
    int unit, 
    int size, 
    uint32 flags, 
    bcm_pkt_t **pkt_buf);

/* Deallocate a packet structure and packet data. */
extern int bcm_pkt_free(
    int unit, 
    bcm_pkt_t *pkt);

/* Allocate or deallocate an array of packets. */
extern int bcm_pkt_blk_alloc(
    int unit, 
    int count, 
    int size, 
    uint32 flags, 
    bcm_pkt_t ***packet_array);

/* Allocate or deallocate an array of packets. */
extern int bcm_pkt_blk_free(
    int unit, 
    bcm_pkt_t **pkt, 
    int count);

/* Allocate or deallocate a packet structure using =bcm_rx_alloc. */
extern int bcm_pkt_rx_alloc(
    int unit, 
    int len, 
    bcm_pkt_t **pkt_buf);

/* Allocate or deallocate a packet structure using =bcm_rx_alloc. */
extern int bcm_pkt_rx_free(
    int unit, 
    bcm_pkt_t *pkt);

/* Initialize a BCM packet structure. */
extern void bcm_pkt_t_init(
    bcm_pkt_t *pkt);

/* Initialize a BCM packet block structure. */
extern void bcm_pkt_blk_t_init(
    bcm_pkt_blk_t *pkt_blk);

/* Set ECMP group field in the pkt structure */
extern int bcm_pkt_ecmp_grp_set(
    int unit, 
    bcm_pkt_t *pkt, 
    bcm_if_t ecmp_group_id);

/* Set ECMP member field in the pkt structure */
extern int bcm_pkt_ecmp_member_set(
    int unit, 
    bcm_pkt_t *pkt, 
    bcm_if_t ecmp_group_id, 
    bcm_if_t ecmp_member_id);

/* Set nexthop field in the pkt structure */
extern int bcm_pkt_nexthop_set(
    int unit, 
    bcm_pkt_t *pkt, 
    bcm_if_t nexthop_id);

/* Initialize a BCM RX reasons structure. */
extern void bcm_rx_reasons_t_init(
    bcm_rx_reasons_t *reasons);

/* Packet drop reasons */
typedef enum bcm_pkt_drop_reason_e {
    bcmPktDropReasonEgressVlanTranslateMiss = 0, /* Packet marked for drop due to egress
                                           vlan translate miss */
    bcmPktDropReasonEgressTunnelSnap = 1, /* SNAP packet dropped becasuse it is
                                           being tunneled */
    bcmPktDropReasonEgressColorSelect = 2, /* If control bcmSwitchColorSelect is
                                           not set to BCM_COLOR_OUTER_CFI and
                                           the CFI of the packet is derived as
                                           1, then packet is dropped */
    bcmPktDropReasonEgressTtlError = 3, /* Packet marked for drop due to TTL
                                           error */
    bcmPktDropReasonEgressVlanFilter = 4, /* Packet marked for drop due to port
                                           not being a member of VLAN. This
                                           happens when VLAN filtering is
                                           enabled by passing
                                           BCM_PORT_VLAN_MEMBER_EGRESS flags to
                                           bcm_port_vlan_member_set API */
    bcmPktDropReasonEgressStgBlocked = 5, /* Packet marked for drop due to
                                           Spanning tree in blocking state */
    bcmPktDropReasonEgressIPMCSameVlanPrune = 6, /* IPMC packet marked for drop due to
                                           bcmSwitchDropReasonIpmcSameVlanPruning
                                           setting */
    bcmPktDropReasonEgressMmuPurge = 7, /* Packet marked for drop due to MMU
                                           purge */
    bcmPktDropReasonEgressMmuAge = 8,   /* Packet marked for drop due to MMU age */
    bcmPktDropReasonEgressBadTdipDrop = 9, /* Packet marked for drop due to bad
                                           tunnel destination IP in case of 6to4
                                           tunnels and ISATAP tunnels - possible
                                           reasons are incorrect prefix for IPv6
                                           DIP,IPv4 DIP/SIP is
                                           BC/Zero/loopback/Class-D, IPv4 SIP =
                                           DIP */
    bcmPktDropReasonEgressProtection = 10, /* Packets dropped because of protection
                                           drop control */
    bcmPktDropReasonEgressUnknownVlan = 11, /* Packet marked for drop due to unknown
                                           VLAN. This happens when
                                           bcmVlanDropUnknown vlan control is
                                           used to set dropping of unknown VLAN
                                           packets */
    bcmPktDropReasonEgressStgDisabled = 12, /* Packet marked for drop due to
                                           Spanning tree in disable state */
    bcmPktDropReasonEgressHiGig2ReservedPPD = 13, /* Packets dropped because of HiGig2
                                           header with reserved ppd going out on
                                           non-hg2 ports */
    bcmPktDropReasonEgressHigigPlusUnknown = 14, /* Unknown HiGigPlus packet incoming
                                           from hg source and dest is non-hg */
    bcmPktDropReasonEgressL2MtuExceeded = 15, /* Packet dropped because of L2 MTU
                                           checks configured on the port
                                           (physical/virtual) */
    bcmPktDropReasonEgressCompositeError = 16, /* Packet marked for drop due to parity
                                           error from memories or ISM lookup
                                           fail */
    bcmPktDropReasonEgressMulticastPruned = 17, /* L3 Multicast packets marked for drop
                                           due to BCM_L3_MULTICAST_L3_DROP or
                                           BCM_L3_MULTICAST_L2_DROP setting in
                                           the multicast APIs */
    bcmPktDropReasonEgressHigigPlusInvalidModuleId = 18, /* Unknown HiGigPlus packet incoming
                                           from non-hg source and dest is hg and
                                           the modid is greater than 31 */
    bcmPktDropReasonEgressFieldAction = 19, /* Packet dropped due to a egress field
                                           action */
    bcmPktDropReasonEgressSplitHorizon = 20, /* Packet marked for drop due to split
                                           horizon check. This is done by
                                           enabling pruning for DVP network
                                           group via
                                           bcm_switch_network_group_config_set
                                           with
                                           BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE */
    bcmPktDropReasonEgressSVPRemoval = 21, /* Packet marked for drop due to virtual
                                           port pruning - SVP = DVP and VP based
                                           pruning is enabled
                                           (DISABLE_VP_PRUNING is set to 0 in
                                           Source VP table) */
    bcmPktDropReasonEgressNivSrcKnockout = 22, /* Outgoing packet dropped due to NIV
                                           pruning check */
    bcmPktDropReasonEgressNonUCPrune = 23, /* Non-unicast packet marked for drop
                                           due to drop condition set in egress
                                           next hop via
                                           bcm_mpls/l2gre/vxlan_port_add API
                                           with BCM_MPLS/L2GRE/VXLAN_PORT_DROP
                                           flag */
    bcmPktDropReasonEgressTrillPacket = 24, /* Trill packets being sent on non-trill
                                           ports */
    bcmPktDropReasonEgressPacketTooSmall = 25, /* Packet dropped since it it too small */
    bcmPktDropReasonEgressNonTrillPacket = 26, /* Non-Trill packets being sent on trill
                                           ports */
    bcmPktDropReasonEgressVpLagPruned = 27, /* Packet dropped because of VP lag
                                           pruning checks in the egress */
    bcmPktDropReasonEgressInvalid1588Packet = 28, /* Packet dropped since it is a invalid
                                           1588 packet */
    bcmPktDropReasonEgressInvalidFcoePacket = 29, /* Packet dropped since it is a invalid
                                           FCOE packet */
    bcmPktDropReasonEgressInvalidCnmPacket = 30, /* Packet dropped since it is a invalid
                                           CNM packet */
    bcmPktDropReasonEgressOriginalPacket = 31, /* Packet dropped in egress due to user
                                           configuration */
    bcmPktDropReasonCount = 32          /* Always Last. Not a usable value */
} bcm_pkt_drop_reason_t;

typedef struct bcm_pkt_drop_reasons_s {
    SHR_BITDCL rbits[_SHR_BITDCLSIZE(bcmPktDropReasonCount)]; 
} bcm_pkt_drop_reasons_t;

/* 
 * Macro to check if a reason (bcmPktDropReason*) is included in a set of
 * reasons (bcm_pkt_drop_reasons_t).
 * 
 * Returns:
 *   zero     => reason is not included in the set
 *   non-zero => reason is included in the set
 */
#define BCM_PKT_DROP_REASON_GET(_drop_reasons, _drop_reason)  \
   SHR_BITGET(((_drop_reasons).rbits), (_drop_reason)) 

/* 
 * Macro to add a reason (bcmPktDropReason*) to a set of reasons
 * (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_SET(_drop_reasons, _drop_reason)  \
   SHR_BITSET(((_drop_reasons).rbits), (_drop_reason)) 

/* 
 * Macro to add all reasons (bcmPktDropReason*) to a set of reasons
 * (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_SET_ALL(_drop_reasons)  \
   SHR_BITSET_RANGE(((_drop_reasons).rbits), 0, bcmPktDropReasonCount) 

/* 
 * Macro to clear a reason (bcmPktDropReason*) from a set of reasons
 * (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_CLEAR(_drop_reasons, _drop_reason)  \
   SHR_BITCLR(((_drop_reasons).rbits), (_drop_reason)) 

/* Macro to clear a set of reasons (bcm_pkt_drop_reasons_t). */
#define BCM_PKT_DROP_REASON_CLEAR_ALL(_drop_reasons)  \
   SHR_BITCLR_RANGE(((_drop_reasons).rbits), 0, bcmPktDropReasonCount) 

/* Macro to check if a set of reasons (bcm_pkt_drop_reasons_t) is empty. */
#define BCM_PKT_DROP_REASON_IS_NULL(_drop_reasons)  \
   SHR_BITNULL_RANGE(((_drop_reasons).rbits), 0, bcmPktDropReasonCount) 

/* Macro to itterate over a set of reasons (bcm_pkt_drop_reasons_t). */
#define BCM_PKT_DROP_REASON_ITER(_drop_reasons, _drop_reason)  \
   for(_drop_reason = bcmPktDropReasonEgressVlanTranslateMiss; _drop_reason < (int)bcmPktDropReasonCount; _drop_reason++) if(SHR_BITGET(((_drop_reasons).rbits), (_drop_reason))) 

/* 
 * Macro to count number of active reasons in a set of reasons
 * (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_COUNT(_drop_reasons, count)  \
                SHR_BITCOUNT_RANGE(((_drop_reasons).rbits), count, 0, bcmPktDropReasonCount)
 

/* Macro to compare two sets of reasons (bcm_pkt_drop_reasons_t). */
#define BCM_PKT_DROP_REASON_EQ(_drop_reasons1, _drop_reasons2)  \
   SHR_BITEQ_RANGE(((_drop_reasons1).rbits), ((_drop_reasons2).rbits), 0, bcmPktDropReasonCount)
 

/* 
 * Macro to negative compare two sets of reasons
 * (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_NEQ(_drop_reasons1, _drop_reasons2)  \
                (!SHR_BITEQ_RANGE(((_drop_reasons1).rbits), ((_drop_reasons2).rbits), 0, bcmPktDropReasonCount))
 

/* 
 * Macro to perform logical AND operation on two sets of reasons
 * (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_AND(_drop_reasons1, _drop_reasons2)  \
                SHR_BITAND_RANGE(((_drop_reasons1).rbits), ((_drop_reasons2).rbits), 0, bcmPktDropReasonCount, ((_drop_reasons1).rbits))
 

/* 
 * Macro to perform logical OR operation on two sets of reasons
 * (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_OR(_drop_reasons1, _drop_reasons2)  \
                SHR_BITOR_RANGE(((_drop_reasons1).rbits), ((_drop_reasons2).rbits), 0, bcmPktDropReasonCount, ((_drop_reasons1).rbits))
 

/* 
 * Macro to remove set of reasons2 (bcm_pkt_drop_reasons_t) from set of
 * reasons1 (bcm_pkt_drop_reasons_t).
 */
#define BCM_PKT_DROP_REASON_REMOVE(_drop_reasons1, _drop_reasons2)  \
   SHR_BITREMOVE_RANGE(((_drop_reasons1).rbits), ((_drop_reasons2).rbits), 0, bcmPktDropReasonCount, ((_drop_reasons1).rbits)); 

/* Macro to negate two sets of reasons (bcm_pkt_drop_reasons_t). */
#define BCM_PKT_DROP_REASON_NEGATE(_drop_reasons1, _drop_reasons2)  \
                SHR_BITNEGATE_RANGE(((_drop_reasons2).rbits), 0, bcmPktDropReasonCount, ((_drop_reasons1).rbits));
 

#endif /* __BCM_PKT_H__ */
