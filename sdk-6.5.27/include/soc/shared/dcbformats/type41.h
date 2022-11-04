/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        soc/shared/dcbformats/type41.h
 * Purpose:     Define dma control block (DCB) format for a type41 DCB
 *              used by the 53650 (Wolfhound3_plus)
 *
 *              This file is shared between the SDK and the embedded applications.
 */

#ifndef _SOC_SHARED_DCBFORMATS_TYPE41_H
#define _SOC_SHARED_DCBFORMATS_TYPE41_H

/*
 * DMA Control Block - Type 41
 * Used on 53650 devices
 * 4 words
 */
typedef struct {
    uint32  addr_lo;                /* T41.0: physical address, lower 32 bits */
    uint32  addr_hi;                /* T41.0: physical address, upper 32 bits */

#ifdef  LE_HOST
    uint32  c_count:16,                 /* Requested byte count */
            c_chain:1,                  /* Chaining */
            c_sg:1,                     /* Scatter Gather */
            c_reload:1,                 /* Reload */
            c_hg:1,                     /* Higig (TX) */
            :2,                         /* reserved */
            c_purge:1,                  /* Purge packet (TX) */
            c_intr:2,                   /* Desc Intr Mode */
            c_desc:4,                   /* descriptors remaining */
            c_desc_status:1,            /* disable descriptor status write */
            :2;                         /* reserved */
#else
    uint32  :2,                         /* resered */
            c_desc_status:1,            /* disable descriptor status write */
            c_desc:4,                   /* descriptors remaining */
            c_intr:2,                   /* Desc Intr Mode */
            c_purge:1,                  /* Purge packet (TX) */
            :2,                         /* reserved */
            c_hg:1,                     /* Higig (TX) */
            c_reload:1,                 /* Reload */
            c_sg:1,                     /* Scatter Gather */
            c_chain:1,                  /* Chaining */
            c_count:16;                 /* Requested byte count */
#endif  /* LE_HOST */

#ifdef  LE_HOST
    uint32  count:16,                   /* Transferred byte count */
            end:1,                      /* End bit (RX) */
            start:1,                    /* Start bit (RX) */
            error:1,                    /* Cell Error (RX) */
            ecc_error:1,                /* packet ECC Error (RX) */
            :11,                        /* Reserved */
            done:1;                     /* Descriptor Done */
#else
    uint32  done:1,                     /* Descriptor Done */
            :11,                        /* Reserved */
            ecc_error:1,                /* Packet ECC Error (RX) */
            error:1,                    /* Cell Error (RX) */
            start:1,                    /* Start bit (RX) */
            end:1,                      /* End bit (RX) */
            count:16;                   /* Transferred byte count */
#endif
} dcb41_t;

/* EP_TO_CPU_HDR
 * - MODULE_HDR               [288:415] WORD9 - WORD12
 * - EP_TO_CPU_COMPOSITES_BUS [  0:287] WORD0 - WORD8
 */
typedef struct {
    uint32  mh0;                        /* Module Header word 0 */
                                        /* (word 12 in EP_TO_CPU_HDR) */
    uint32  mh1;                        /* Module Header word 1 */
                                        /* (word 11 in EP_TO_CPU_HDR) */
    uint32  mh2;                        /* Module Header word 2 */
                                        /* (word 10 in EP_TO_CPU_HDR) */
    uint32  mh3;                        /* Module Header word 3 */
                                        /* (word 9 in EP_TO_CPU_HDR) */

                                        /* (word 8 in EP_TO_CPU_HDR) */
                                        /* word 8 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  reason_hi;                  /* CPU opcode (hi bits) */

                                        /* (word 7 in EP_TO_CPU_HDR) */
                                        /* word 7 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  reason;                     /* CPU opcode (low bits) */

#ifdef  LE_HOST
                                        /* (word 6 in EP_TO_CPU_HDR) */
                                        /* word 6 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  cpu_cos:6,          /* Queue# used in MMU for cpu port */
                    :9,                 /* reserved.*/
                    ep_mirror:1,        /* ep mirror to CPU */
                    cpu_opcode_type:4,  /* reason code TYPE(sending to CPU) */
                    outer_vid:12;       /* Outer VLAN ID */
        } overlay1;
        struct {
            uint32  queue_num:15,       /* Queue number used in MMU, PBE field */
                    ep_mirror:1,        /* ep mirror to CPU */
                    cpu_opcode_type:4,  /* reason code TYPE(sending to CPU) */
                    outer_vid:12;       /* Outer VLAN ID */
        } overlay2;
    } word6;

                                        /* (word 5 in EP_TO_CPU_HDR) */
                                        /* word 5 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  srcport:7,                  /* Source Port Number, PBE field */
            :1,                         /* Reserved */
            pkt_len:14,                 /* Packet length */
            match_rule:8,               /* Matched FP rule */
            hgi:2;                      /* Higig+ module header field */

                                        /* (word 4 in EP_TO_CPU_HDR) */
                                        /* word 4 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  inner_pri:3,                /* Inner priority */
            dvp_nhi_sel:1,              /* DVP overlay indicator */
            regen_crc:1,                /* Packet modified and needs new CRC */
            repl_nhi:14,                /* Replication or Next Hop index */
            vrf:4,                      /* Virtual Routing and Forwarding */
            svtag_present:1,            /* Indicate presence of SVTAG */
            vfi_valid:1,                /* Validates VFI field */
            olp_encap_oam_pkt:1,        /* OLP encapsulated OAM packet */
            :6;                         /* Reserved */

                                        /* (word 3 in EP_TO_CPU_HDR) */
                                        /* word 3 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  dscp:6,             /* New DSCP */
                    chg_tos:1,          /* DSCP has been changed by HW */
                    vntag_action:2,     /* VN tag action */
                    outer_cfi:1,        /* Outer Canoncial Format Indicator */
                    outer_pri:3,        /* Outer priority */
                    decap_tunnel_type:4,/* Tunnel type that was decapsulated */
                    :1,                 /* Reserved */
                    inner_vid:12,       /* Inner VLAN ID */
                    inner_cfi:1,        /* Inner Canoncial Format Indicator */
                    mcq:1;              /* Multicast queue */
        } overlay1;
        struct {
            uint32  special_pkt_type:3, /* Special packet type (OAM, BFD) */
                    :29;
        } overlay2;
    } word3;

                                        /* (word 2 in EP_TO_CPU_HDR) */
                                        /* word 2 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  timestamp;                  /* Timestamp */

                                        /* (word 1 in EP_TO_CPU_HDR) */
                                        /* word 1 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  tag_status:2,               /* Ingress incoming tag status */
            otag_action:2,              /* Ingress outer tag action */
            itag_action:2,              /* Ingress inner tag action */
            service_tag:1,              /* SD tag present */
            switch_pkt:1,               /* Switched packet */
            all_switch_drop:1,          /* All switched copies dropped */
            hg_type:1,                  /* 0: Higig+, 1: Higig2 */
            src_hg:1,                   /* Source is Higig */
            l3routed:1,                 /* Any IP routed packet */
            l3only:1,                   /* L3 only IPMC packet */
            replicated:1,               /* Replicated copy */
            hg2_ext_hdr:1,              /* Extended Higig2 header valid */
            special_pkt:1,              /* Special Packet indicator */
            do_not_change_ttl:1,        /* Do not change TTL */
            bpdu:1,                     /* BPDU Packet */
            timestamp_type:2,           /* Timestamp type */
            loopback_pkt_type:3,        /* Loopback packet type */
            :1,                         /* Reserved */
            mtp_index:5,                /* MTP index */
            ecn:2,                      /* New ECN value */
            chg_ecn:1;                  /* ECN changed */

                                        /* (word 0 in EP_TO_CPU_HDR) */
                                        /* word 0 in EP_TO_CPU_COMPOSITES_BUS */

    union {
        uint32  timestamp_hi;           /* Timestamp upper bits */
        struct {                        /* EH_TYPE_0 */
            uint32  eh_queue_tag:16,
                    :3,
                    eh_tag_type:2,
                    eh_seg_sel:3,
                    :3,
                    os1588_done:1,
                    eh_type:4;
        } overlay1;
        struct {                        /* EH_TYPE_1 */
            uint32  classid:9,
                    l3_iif:14,
                    classid_type:4,
                    :5;
        } overlay2;
        struct {                        /* EH_TYPE 2 */
            uint32  eh_queue_tag: 14,
                    classid:9,
                    classid_type:4,
                    :5;
        } overlay3;
        struct {
            uint32  rx_bfd_session_index:12,    /* Rx BFD pkt session ID */
                    rx_bfd_start_offset_type:2, /* Rx BFD pkt st offset type */
                    rx_bfd_start_offset:8,      /* Rx BFD pkt start offset */
                    :10;                        /* Reserved */
        } overlay4;
    } word0;

#else
                                        /* (word 6 in EP_TO_CPU_HDR) */
                                        /* word 6 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  outer_vid:12,       /* Outer VLAN ID */
                    cpu_opcode_type:4,  /* reason code TYPE(sending to CPU) */
                    ep_mirror_bit:1,    /* ep mirror to CPU */
                    :9,                 /* reserved.*/
                    cpu_cos:6;          /* Queue# used in MMU for cpu port */
        } overlay1;
        struct {
            uint32  outer_vid:12,       /* Outer VLAN ID */
                    cpu_opcode_type:4,  /* reason code TYPE(sending to CPU) */
                    ep_mirror_bit:1,    /* ep mirror to CPU */
                    queue_num:15;       /* Queue number used in MMU, PBE field */
        } overlay2;
    } word6;

                                        /* (word 5 in EP_TO_CPU_HDR) */
                                        /* word 5 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  hgi:2,                      /* Higig+ module header field */
            match_rule:8,               /* Matched FP Rule */
            pkt_len:14,                 /* Packet length */
            :1,                         /* Reserved */
            srcport:7;                  /* Source Port Number, PBE field */

                                        /* (word 4 in EP_TO_CPU_HDR) */
                                        /* word 4 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  :6,                         /* Reserved */
            olp_encap_oam_pkt:1,        /* OLP encapsulated OAM packet */
            vfi_valid:1,                /* Validates VFI field */
            svtag_present:1,            /* Indicate presence of SVTAG */
            vrf:4,                      /* Virtual Routing and Forwarding */
            repl_nhi:14,                /* Replication or Next Hop index */
            regen_crc:1,                /* Packet modified and needs new CRC */
            dvp_nhi_sel:1,              /* DVP overlay indicator */
            inner_pri:3;                /* Inner priority */

                                        /* (word 3 in EP_TO_CPU_HDR) */
                                        /* word 3 in EP_TO_CPU_COMPOSITES_BUS */
    union {
        struct {
            uint32  mcq:1,              /* Multicast queue */
                    inner_cfi:1,        /* Inner Canoncial Format Indicator */
                    inner_vid:12,       /* Inner VLAN ID */
                    :1,                 /* Reserved */
                    decap_tunnel_type:4,/* Tunnel type that was decapsulated */
                    outer_pri:3,        /* Outer priority */
                    outer_cfi:1,        /* Outer Canoncial Format Indicator */
                    vntag_action:2,     /* VN tag action */
                    chg_tos:1,          /* DSCP has been changed by HW */
                    dscp:6;             /* New DSCP */
        } overlay1;
        struct {
            struct {
            uint32  :29,
                    special_pkt_type:3; /* Special packet type (OAM, BFD) */
        } overlay2;
    } word3;

                                        /* (word 2 in EP_TO_CPU_HDR) */
                                        /* word 2 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  timestamp;                  /* Timestamp */

                                        /* (word 1 in EP_TO_CPU_HDR) */
                                        /* word 1 in EP_TO_CPU_COMPOSITES_BUS */
    uint32  chg_ecn:1,                  /* ECN changed */
            ecn:2,                      /* New ECN value */
            mtp_index:5,                /* MTP index */
            :1,                         /* Reserved */
            loopback_pkt_type:3,        /* Loopback packet type */
            timestamp_type:2,           /* Timestamp type */
            bpdu:1,                     /* BPDU Packet */
            do_not_change_ttl:1,        /* Do not change TTL */
            special_pkt:1,              /* Special Packet indicator */
            hg2_ext_hdr:1,              /* Extended Higig2 header valid */
            replicated:1,               /* Replicated copy */
            l3only:1,                   /* L3 only IPMC packet */
            l3routed:1,                 /* Any IP routed packet */
            src_hg:1,                   /* Source is Higig */
            hg_type:1,                  /* 0: Higig+, 1: Higig2 */
            all_switch_drop:1,          /* All switched copies dropped */
            switch_pkt:1,               /* Switched packet */
            service_tag:1,              /* SD tag present */
            itag_action:2,              /* Ingress inner tag action */
            otag_action:2,              /* Ingress outer tag action */
            tag_status:2;               /* Ingress incoming tag status */

                                        /* (word 0 in EP_TO_CPU_HDR) */
                                        /* word 0 in EP_TO_CPU_COMPOSITES_BUS */

    union {                             /* T32.14 (word 0 in EP_TO_CPU_HDR) */
        uint32  timestamp_hi;           /* Timestamp upper bits */
        struct {                        /* EH_TYPE_0 */
            uint32  eh_type:4,
                    os1588_done:1,
                    :3,
                    eh_seg_sel:3,
                    eh_tag_type:2,
                    :3,
                    eh_queue_tag:16;
        } overlay1;
        struct {                        /* EH_TYPE_1 */
            uint32  :5,
                    classid_type:4,
                    l3_iif:14,
                    classid:9;
        } overlay2;
        struct {                        /* EH_TYPE 2 */
            uint32  :5,
                    classid_type:4,
                    classid:9,
                    eh_queue_tag: 14;
        } overlay3;
        struct {
            uint32  :10,                        /* Reserved */
                    rx_bfd_start_offset:8,      /* Rx BFD pkt start offset */
                    rx_bfd_start_offset_type:2, /* Rx BFD pkt st offset type */
                    rx_bfd_session_index:12;    /* Rx BFD pkt session ID */
        } overlay4;
    } word0;
#endif

    uint32 rsvd;                        /* padding to get header to 56 bytes */
} ep_to_cpu_hdr41_t;

#define SOC_CPU_OPCODE_TYPE_IP_0        0
#define SOC_CPU_OPCODE_TYPE_IP_1        1
#define SOC_CPU_OPCODE_TYPE_EP          2
#define SOC_CPU_OPCODE_TYPE_NLF         3

#endif /* _SOC_SHARED_DCBFORMATS_TYPE41_H */
