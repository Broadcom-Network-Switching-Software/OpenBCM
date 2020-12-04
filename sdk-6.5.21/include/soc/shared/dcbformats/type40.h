/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc/shared/dcbformats/type40.h
 * Purpose:     Define dma control block (DCB) format for a type40 DCB
 *              used by the 56070 (FireLight)
 *
 *              This file is shared between the SDK and the embedded applications.
 */

#ifndef _SOC_SHARED_DCBFORMATS_TYPE40_H
#define _SOC_SHARED_DCBFORMATS_TYPE40_H

/*
 * DMA Control Block - Type 40
 * Used on 56070 devices
 * 16 words
 */
typedef struct {
        uint32  addr_lo;                    /* T36.0: physical address, lower 32 bits */
        uint32  addr_hi;                    /* T36.0: physical address, upper 32 bits */

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

} dcb40_t;

/* EP_TO_CPU_HDR
 * - MODULE_HDR               [288:415] WORD9 - WORD12
 * - EP_TO_CPU_COMPOSITES_BUS [  0:287] WORD0 - WORD8
 */
typedef struct {
        uint32  mh0;                    /* T40.12: Module Header word 0 */
        uint32  mh1;                    /* T40.11: Module Header word 1 */
        uint32  mh2;                    /* T40.10: Module Header word 2 */
        uint32  mh3;                    /* T40.9: Module Header word 3 */

        uint32  reason_hi;              /* T40.8: CPU opcode (hi bits) */
        uint32  reason;                 /* T40.7: CPU opcode (low bits) */

#ifdef  LE_HOST
    union {                             /* T40.6 */
        struct {
            uint32  :16,                /* Reserved */
                    cpu_opcode_type:4,  /* CPU opcode type */
                    outer_vid:12;       /* Outer VLAN ID */
        } overlay1;
        struct {
            uint32  cpu_cos:6,          /* CPU COS */
                    regen_crc:1,        /* Packet modified and needs new CRC */
                    :25;                /* Reserved */
        } overlay2;
    } word6;

                                        /* T40.5 */
        uint32  srcport:8,              /* Source port number */
                pkt_len:14,             /* Packet length */
                match_rule:8,           /* Matched FP rule */
                hgi:2;                  /* Higig Interface Format Indicator */
#else
    union {                             /* T40.6 */
        struct {
            uint32  outer_vid:12,       /* Outer VLAN ID */
                    cpu_opcode_type:4,  /* CPU opcode type */
                    :16;                /* Reserved */
        } overlay1;
        struct {
            uint32  :25,                /* Reserved */
                    regen_crc:1,        /* Packet modified and needs new CRC */
                    cpu_cos:6;          /* CPU COS */
        } overlay2;
    } word6;

                                        /* T40.5 */
        uint32  hgi:2,                  /* Higig Interface Format Indicator */
                match_rule:8,           /* Matched FP rule */
                pkt_len:14,             /* Packet length */
                srcport:8;              /* Source port number */
#endif

#ifdef  LE_HOST
                                        /* T40.4 */
        uint32  inner_pri:3,            /* Inner priority */
                repl_nhi:13,            /* Replication or Next Hop index */
                :5,                     /* reserved.*/
                shaping_cos_sel:2,      /* */
                vfi_valid:1,            /* Validates VFI field */
                em_mtp_index:4,         /* em mirror-to-port index */
                oam_command:4;          /* Special packet type (OAM, BFD) */

                                        /* T40.3 */
        uint32  dscp:6,                 /* New DSCP (+ oam pkt type:3) */
                chg_tos:1,              /* DSCP has been changed by HW */
                imirror:1,              /* Ingress Mirroring PBE field*/
                emirror:1,              /* Egress Mirroring, PBE field */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_pri:3,            /* Outer priority */
                :5,                     /* Reserved */
                inner_vid:12,           /* Inner VLAN ID */
                inner_cfi:1,            /* Inner Canoncial Format Indicator */
                replicated:1;           /* Replicated copy */



    union {                             /* T40.2 */
        struct {
            uint32  timestamp;          /* Timestamp */
        } overlay1;
        struct {
            uint32  vfi:14,             /* VFI or FID */
                    :18;                /* Reserved */
        } overlay2;
    } word2;

                                        /* T40.1 */
        uint32  itag_status:2,          /* Ingress Inner tag status */
                otag_action:2,          /* Ingress Outer tag action */
                itag_action:2,          /* Ingress Inner tag action */
                service_tag:1,          /* SD tag present */
                switch_pkt:1,           /* Switched packet */
                hg_lookup:1,            /* Higig lookup */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                src_hg:1,               /* Source is Higig */
                l3routed:1,             /* Any IP routed packet */
                l3only:1,               /* L3 only IPMC packet */
                vntag_action:2,         /* VNTAG/ETAG*/
                oam_pkt:1,              /* OAM Packet indicator */
                do_not_change_ttl:1,    /* Do not change TTL */
                bpdu:1,                 /* BPDU Packet */
                timestamp_type:2,       /* Timestamp type */
                cos:4,                  /* Port queue number */
                mtp_index:5,            /* IM MTP index, PBE field */
                ecn:2,                  /* New ECN value */
                chg_ecn:1;              /* ECN changed */

                                        /* T40.0 */
        uint32  timestamp_hi;           /* Timestamp upper bits */
#else
                                        /* T40.4 */
        uint32  oam_command:4,          /* Special packet type (OAM, BFD) */
                em_mtp_index:4,         /* em mirror-to-port index */
                vfi_valid:1,            /* Validates VFI field */
                shaping_cos_sel:2,      /* */
                :5,                     /* reserved.*/
                repl_nhi:13,            /* Replication or Next Hop index */
                inner_pri:3;            /* Inner priority */

                                        /* T40.3 */
        uint32  replicated:1,           /* Replicated copy */
                inner_cfi:1,            /* Inner Canoncial Format Indicator */
                inner_vid:12,           /* Inner VLAN ID */
                :5,                     /* Reserved */
                outer_pri:3,            /* Outer priority */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                emirror:1,              /* Egress Mirroring, PBE field */
                imirror:1,              /* Ingress Mirroring PBE field*/
                chg_tos:1,              /* DSCP has been changed by HW */
                dscp:6;                 /* New DSCP (+ oam pkt type:3) */

    union {                             /* T40.2 */
        struct {
            uint32  timestamp;          /* Timestamp */
        } overlay1;
        struct {
            uint32  :18,                /* Reserved */
                    vfi:14;             /* VFI or FID */
        } overlay2;
    } word2;

                                        /* T40.1 */
        uint32  chg_ecn:1,              /* ECN changed */
                ecn:2,                  /* New ECN value */
                mtp_index:5,            /* IM MTP index, PBE field */
                cos:4,                  /* Port queue number */
                timestamp_type:2,       /* Timestamp type */
                bpdu:1,                 /* BPDU Packet */
                do_not_change_ttl:1,    /* Do not change TTL */
                oam_pkt:1,              /* OAM Packet indicator */
                vntag_action:2,         /* VNTAG/ETAG*/
                l3only:1,               /* L3 only IPMC packet */
                l3routed:1,             /* Any IP routed packet */
                src_hg:1,               /* Source is Higig */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                hg_lookup:1,            /* Higig lookup */
                switch_pkt:1,           /* Switched packet */
                service_tag:1,          /* SD tag present */
                itag_action:2,          /* Ingress Inner tag action */
                otag_action:2,          /* Ingress Outer tag action */
                itag_status:2;          /* Ingress Inner tag status */

                                        /* T40.0 */
        uint32  timestamp_hi;           /* Timestamp upper bits */
#endif
        uint32 rsvd;                  /* 4 byte padding to make it 56 bytes */
} ep_to_cpu_hdr40_t;

#define SOC_CPU_OPCODE_TYPE_IP_0        0
#define SOC_CPU_OPCODE_TYPE_IP_1        1
#define SOC_CPU_OPCODE_TYPE_EP          2
#define SOC_CPU_OPCODE_TYPE_NLF         3

#endif /* _SOC_SHARED_DCBFORMATS_TYPE40_H */
