/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        soc/dcbformats.h
 * Purpose:     Define dma control block (DCB) formats for various chips.
 *              This information is only needed in soc/dcb.c, which
 *              provides a uniform interface to the reset of software
 *              for controlling DCBs.
 *
 * There are currently 32 DMA Control block formats
 * (also known as DMA descriptors):
 *
 *      Type 2          5670/75 (TX)
 *      Type 3          5690 aka DRACO1
 *      Type 4          5670/75 (RX) aka HERCULES
 *      Type 5          5673 aka LYNX
 *      Type 6          5665 aka TUCANA
 *      Type 7          5695 aka DRACO15
 *      Type 9          56504 aka XGS3 (Firebolt)
 *      Type 10         56601 aka XGS3 (Easyrider)
 *      Type 11         56800 aka XGS3 (HUMV)
 *      Type 12         56218 aka XGS3 (Raptor) (Derived from DCB 9)
 *      Type 13         56514 aka XGS3 (Firebolt2) (Derived from DCB 9)
 *      Type 14         56624 & 56680 aka XGS3 (Triumph & Valkyrie)
 *      Type 15         56224 aka XGS3 (Raven A0) (Derived from DCB 12)
 *      Type 16         56820 aka XGS3 (Scorpion)
 *      Type 17         53314 aka XGS3 (Hawkeye) (Derived from DCB 15)
 *      Type 18         56224 aka XGS3 (Raven B0) (Derived from DCB 15)
 *      Type 19         56634/56524/56685 aka XGS3 (Triumph2/Apollo/Valkyrie2)
 *      Type 20         56334 aka XGS3 (Enduro) (Derived from DCB 19)
 *      Type 21         56840 aka XGS3 (Trident)
 *      Type 22         88732 (SHADOW)
 *      Type 23         56640 aka XGS3 (Triumph3)
 *      Type 24         56440 aka XGS3 (Katana)
 *      Type 25         Obsolete: 88030 aka Caladan3
 *      Type 26         56850 aka XGS3 (Trident2) (Derived from DCB 23)
 *      Type 27         Obsolete: 88230 aka Sirius
 *      Type 29         56450 aka XGX3 (Katana2)
 *      Type 31         53400 aka XGX3 (Greyhound)
 *      Type 32         56960 aka XGX3 (Tomahawk) (Derived from DCB 26)
 *      Type 33         56860 aka XGS3 (Trident2+) (Derived from DCB 26)
 *      Type 34         56160 aka XGX3 (Hurricane3)
 *      Type 35         56560 APACHE
 *      Type 36         56870 (Trident 3)88690 (Jericho 2)
 *      Type 37         53570 aka XGX3 (Greyhound2)
 *      Type 38         56980 (Tomahawk 3)
 *      Type 40         56070 aka XGX3 (FireLight)
 *
 * The 5670/75 uses two different formats depending on whether the DCB is
 * for transmit or receive.
 *
 * The 5695 can be configured to use type 3 or type 7 depending on whether
 * CMIC_CONFIG.EXTENDED_DCB_ENABLE is set or not.
 */

#ifndef _SOC_DCBFORMATS_H
#define _SOC_DCBFORMATS_H

#include <soc/types.h>
#include <soc/defs.h>

#define DCB_STRATA_OPTIM_PURGE  3
#define DCB_STRATA_CRC_LEAVE    0
#define DCB_STRATA_CRC_REGEN    2
#define DCB_XGS_CRC_LEAVE       0
#define DCB_XGS_CRC_REGEN       1

#define DCB_MAX_REQCOUNT        0x7fff          /* 32KB */
#define DCB_MAX_SIZE            (16*4)          /* type 14 */

#if defined(BCM_HERCULES_SUPPORT)
/*
 * DMA Control Block - Type 2
 * Used on STRATA2 devices and on 5670/75 for transmission
 * 8 words
 */
typedef struct {
        uint32  addr;                   /* T2.0: physical address */
                                        /* T2.1: Control 0 (same as T1) */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                :1,
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                :2,
                :1,                     /* Unused (L3/IPX data) */
                c_ip:1,                 /* L3/IP switched packet */
                c_optim:2,              /* DMAR: optimization bits (below) */
                c_crc:2,                /* DMAW: send pkt asis or regen crc */
                c_jumbo:1,              /* DMAR: jumbo packet */
                c_cos:3,                /* DMAR: Class of service */
                c_chain:1;              /* Chaining */
#else
        uint32  c_chain:1,
                c_cos:3,
                c_jumbo:1,
                c_crc:2,
                c_optim:2,
                c_ip:1,
                :1,
                :2,
                c_sg:1,
                c_reload:1,
                :1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  l2ports;                /* T2.2: L2 transmit to ports */
        uint32  utports;                /* T2.3: L2 untagged ports */
        uint32  l3ports;                /* T2.4: L3 transmit to ports */
                                        /* T2.5: Status 0 (same as T1) */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                :4,
                start:1,                /* Start of packet transferred */
                optim:2,                /* Optimization: 0:unused; 1:C bit set;
                                         *  2:frame mismatch; 3:RSVP */
                crc:2,                  /* CRC_LEAVE: valid CRC */
                                        /* CRC_REGEN: invalid CRC */
                :1,
                cos:3,                  /* COS value for packet */
                untagged:1,             /* cmic/egr removed tag */
                end:1,                  /* Last packet or end of packet */
                s0valid:1;              /* Status word valid */
#else
        uint32  s0valid:1,
                end:1,
                untagged:1,
                cos:3,
                :1,
                crc:2,
                optim:2,
                start:1,
                :4,
                count:16;
#endif
                                        /* T2.6: Status 1 (same as T1) */
#ifdef  LE_HOST
        uint32  srcport:6,              /* Source port for this packet */
                :2,
                s1valid:1,              /* status word valid */
                :5,
                reason:18;              /* CPU opcode */
#else
        uint32  reason:18,
                :5,
                s1valid:1,
                :2,
                srcport:6;
#endif
                                        /* T2.7: Status 2 */
#ifdef  LE_HOST
        uint32  remote_port:6,          /* Remote port */
                match_rule:8,           /* Match rule */
                :17,
                s2valid:1;              /* status word valid */
#else
        uint32  s2valid:1,
                :17,
                match_rule:8,
                remote_port:6;
#endif
} dcb2_t;
#endif  /* BCM_HERCULES_SUPPORT */

#ifdef  BCM_DRACO1_SUPPORT
/*
 * DMA Control Block - Type 3
 * Used on 5690 devices
 * 8 words
 */
typedef struct {
        uint32  addr;                   /* T3.0: physical address */
                                        /* T3.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:15,             /* Requested byte count */
                c_crc:1,                /* send pkt asis or regen crc */
                c_destport_hi:1,        /* Destination port [4] */
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                c_destport_lo:4,        /* Destination port [3:0] */
                c_destmod:5,            /* Destination module */
                c_cos:3,                /* class of service */
                c_chain:1;              /* Chaining */
#else
        uint32  c_chain:1,
                c_cos:3,
                c_destmod:5,
                c_destport_lo:4,
                c_sg:1,
                c_reload:1,
                c_destport_hi:1,
                c_crc:1,
                c_count:15;
#endif  /* LE_HOST */
                                        /* T3.2: L2 transmit to ports */
#ifdef  LE_HOST
        uint32  l2ports:31,
                c_opcode0:1;            /* hdr opcode [0] */
#else
        uint32  c_opcode0:1,
                l2ports:31;
#endif
                                        /* T3.3: L2 untagged ports */
#ifdef  LE_HOST
        uint32  utports:31,
                c_opcode1:1;            /* hdr opcode [1] */
#else
        uint32  c_opcode1:1,
                utports:31;
#endif
                                        /* T3.4: L3 transmit to ports */
#ifdef  LE_HOST
        uint32  l3ports:31,
                c_opcode2:1;            /* hdr opcode [2] */
#else
        uint32  c_opcode2:1,
                l3ports:31;
#endif
                                        /* T3.5: Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                crc:1,                  /* Pkt had invalid CRC */
                tagged:1,               /* Pkt was tagged */
                srcport_hi:1,           /* Source port [4] */
                l3:1,                   /* Pkt was l3 switched */
                start:1,                /* Start of pkt */
                cos:3,                  /* cpu class of service */
                srcport_lo:5,           /* Source Port [3:0] */
                error:1,                /* Error occurred */
                end:1,                  /* End of pkt */
                s0valid:1;              /* Status word valid */
#else
        uint32  s0valid:1,
                end:1,
                error:1,
                srcport_lo:5,
                cos:3,
                start:1,
                l3:1,
                srcport_hi:1,
                tagged:1,
                crc:1,
                count:16;
#endif
                                        /* T3.6: Status 1 */
#ifdef  LE_HOST
        uint32  ingport:4,              /* local source (ingress) port */
                opcode:3,               /* hdr opcode */
                prio_lo:1,              /* hdr priority [0] */
                s1valid:1,              /* status word valid */
                prio_hi:2,              /* hdr priority [2:1] */
                srcmod:5,               /* source module */
                reason:16;              /* cpu opcode */
#else
        uint32  reason:16,
                srcmod:5,
                prio_hi:2,
                s1valid:1,
                prio_lo:1,
                opcode:3,
                ingport:4;
#endif
                                        /* T3.7: Status 2 */
#ifdef  LE_HOST
        uint32  class_tag:16,           /* classification tag */
                match_rule:7,           /* match rule */
                destport:5,             /* destination port */
                destmod_lo:3,           /* destination module [2:0] */
                s2valid:1;              /* status word valid */
#else
        uint32  s2valid:1,
                destmod_lo:3,
                destport:5,
                match_rule:7,
                class_tag:16;
#endif
} dcb3_t;
#endif  /* BCM_DRACO1_SUPPORT */

#ifdef  BCM_HERCULES_SUPPORT
/*
 * DMA Control Block - Type 4
 * Used on 5670/75 devices for RX
 * 8 words
 */
typedef struct {
        uint32  addr;                   /* T4.0: physical address */
                                        /* T4.1: Control 0 (same as T1) */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                :1,
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                :2,
                :1,                     /* Unused (L3/IPX data) */
                c_ip:1,                 /* L3/IP switched packet */
                c_optim:2,              /* DMAR: optimization bits (below) */
                c_crc:2,                /* DMAW: send pkt asis or regen crc */
                c_jumbo:1,              /* DMAR: jumbo packet */
                c_cos:3,                /* DMAR: Class of service */
                c_chain:1;              /* Chaining */
#else
        uint32  c_chain:1,
                c_cos:3,
                c_jumbo:1,
                c_crc:2,
                c_optim:2,
                c_ip:1,
                :1,
                :2,
                c_sg:1,
                c_reload:1,
                :1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  l2ports;                /* T4.2: L2 transmit to ports */
        uint32  utports;                /* T4.3: L2 untagged ports */
        uint32  l3ports;                /* T4.4: L3 transmit to ports */
                                        /* T4.5: Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                ip:2,                   /* L3/IP switched packet (always 0) */
                utvalid:1,              /* Untag bitmap received (always 0) */
                l3:1,                   /* Pkt was l3 switched (always 0) */
                start:1,                /* Start of pkt */
                optim:2,                /* Optimization bits (always 0) */
                crc:2,                  /* Pkt had invalid CRC (always 0) */
                jumbo:1,                /* Jumbo packet (always 0) */
                cos:3,                  /* cpu class of service */
                error:1,                /* Error occurred */
                end:1,                  /* End of pkt */
                s0valid:1;              /* Status word valid */
#else
        uint32  s0valid:1,
                end:1,
                error:1,
                cos:3,
                jumbo:1,
                crc:2,
                optim:2,
                start:1,
                l3:1,
                utvalid:1,
                ip:2,
                count:16;
#endif
                                        /* T4.6: Status 1 (same as T3) */
#ifdef  LE_HOST
        uint32  ingport:4,              /* local source (ingress) port */
                opcode:3,               /* hdr opcode */
                prio_lo:1,              /* hdr priority [0] */
                s1valid:1,              /* status word valid */
                prio_hi:2,              /* hdr priority [2:1] */
                srcmod:5,               /* source module */
                reason:16;              /* cpu opcode */
#else
        uint32  reason:16,
                srcmod:5,
                prio_hi:2,
                s1valid:1,
                prio_lo:1,
                opcode:3,
                ingport:4;
#endif
                                        /* T4.7: Status 2 (same as T3) */
#ifdef  LE_HOST
        uint32  class_tag:16,           /* classification tag */
                match_rule:7,           /* match rule */
                destport:5,             /* destination port */
                destmod_lo:3,           /* destination module [2:0] */
                s2valid:1;              /* status word valid */
#else
        uint32  s2valid:1,
                destmod_lo:3,
                destport:5,
                match_rule:7,
                class_tag:16;
#endif
} dcb4_t;
#endif  /* BCM_HERCULES_SUPPORT */

#ifdef  BCM_LYNX_SUPPORT
/*
 * DMA Control Block - Type 5
 * Used on 5673 devices
 * 9 words
 */
typedef struct {
        uint32  addr;                   /* T5.0: physical address */
                                        /* T5.1: Control 0 (same as T3) */
#ifdef  LE_HOST
        uint32  c_count:15,             /* Requested byte count */
                c_crc:1,                /* send pkt asis or regen crc */
                c_destport_hi:1,        /* Destination port [4] */
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                c_destport_lo:4,        /* Destination port [3:0] */
                c_destmod:5,            /* Destination module */
                c_cos:3,                /* class of service */
                c_chain:1;              /* Chaining */
#else
        uint32  c_chain:1,
                c_cos:3,
                c_destmod:5,
                c_destport_lo:4,
                c_sg:1,
                c_reload:1,
                c_destport_hi:1,
                c_crc:1,
                c_count:15;
#endif  /* LE_HOST */
                                        /* T5.2: L2 transmit to ports */
#ifdef  LE_HOST
        uint32  l2ports:31,
                c_opcode0:1;            /* hdr opcode [0] */
#else
        uint32  c_opcode0:1,
                l2ports:31;
#endif
                                        /* T5.3: L2 untagged ports */
#ifdef  LE_HOST
        uint32  utports:31,
                c_opcode1:1;            /* hdr opcode [1] */
#else
        uint32  c_opcode1:1,
                utports:31;
#endif
                                        /* T5.4: L3 transmit to ports */
#ifdef  LE_HOST
        uint32  l3ports:31,
                c_opcode2:1;            /* hdr opcode [2] */
#else
        uint32  c_opcode2:1,
                l3ports:31;
#endif
                                        /* T5.5: Status 0 (same as T3) */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                crc:1,                  /* Pkt had invalid CRC */
                tagged:1,               /* Pkt was tagged */
                srcport_hi:1,           /* Source port [4] */
                l3:1,                   /* Pkt was l3 switched */
                start:1,                /* Start of pkt */
                cos:3,                  /* cpu class of service */
                srcport_lo:5,           /* Source Port [3:0] */
                error:1,                /* Error occurred */
                end:1,                  /* End of pkt */
                s0valid:1;              /* Status word valid */
#else
        uint32  s0valid:1,
                end:1,
                error:1,
                srcport_lo:5,
                cos:3,
                start:1,
                l3:1,
                srcport_hi:1,
                tagged:1,
                crc:1,
                count:16;
#endif
                                        /* T5.6: Status 1 (same as T3) */
#ifdef  LE_HOST
        uint32  ingport:4,              /* local source (ingress) port */
                opcode:3,               /* hdr opcode */
                prio_lo:1,              /* hdr priority [0] */
                s1valid:1,              /* status word valid */
                prio_hi:2,              /* hdr priority [2:1] */
                srcmod:5,               /* source module */
                reason:16;              /* cpu opcode */
#else
        uint32  reason:16,
                srcmod:5,
                prio_hi:2,
                s1valid:1,
                prio_lo:1,
                opcode:3,
                ingport:4;
#endif
                                        /* T5.7: Status 2 (same as T3) */
#ifdef  LE_HOST
        uint32  class_tag:16,           /* classification tag */
                match_rule_lo:7,        /* match rule (renamed from type 3) */
                destport:5,             /* destination port */
                destmod_lo:3,           /* destination module [2:0] */
                s2valid:1;              /* status word valid */
#else
        uint32  s2valid:1,
                destmod_lo:3,
                destport:5,
                match_rule_lo:7,
                class_tag:16;
#endif
                                        /* T5.8: Status 3 */
#ifdef  LE_HOST
        uint32  match_rule:10,          /* match rule */
                :21,
                s3valid:1;              /* status word valid */
#else
        uint32  s3valid:1,
                :21,
                match_rule:10;
#endif
} dcb5_t;
#endif  /* BCM_LYNX_SUPPORT */

#ifdef  BCM_TUCANA_SUPPORT
/*
 * DMA Control Block - Type 6
 * Used on 5665 devices
 * 15 words
 */
typedef struct {
        uint32  addr;                   /* T6.0: physical address */
                                        /* T6.1: Control 0 (same as T3) */
#ifdef  LE_HOST
        uint32  c_count:15,             /* Requested byte count */
                c_crc:1,                /* send pkt asis or regen crc */
                c_destport_hi:1,        /* Destination port [4] */
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                c_destport_lo:4,        /* Destination port [3:0] */
                c_destmod:5,            /* Destination module */
                c_cos:3,                /* class of service */
                c_chain:1;              /* Chaining */
#else
        uint32  c_chain:1,
                c_cos:3,
                c_destmod:5,
                c_destport_lo:4,
                c_sg:1,
                c_reload:1,
                c_destport_hi:1,
                c_crc:1,
                c_count:15;
#endif  /* LE_HOST */
        uint32  l2ports_mod0;           /* T6.2: L2 transmit to ports */
        uint32  l2ports_mod1;           /* T6.3: L2 transmit to ports */
        uint32  utports_mod0;           /* T6.4: L2 untagged ports */
        uint32  utports_mod1;           /* T6.5: L2 untagged ports */
        uint32  l3ports_mod0;           /* T6.6: L3 transmit to ports */
        uint32  l3ports_mod1;           /* T6.7: L3 transmit to ports */
                                        /* T6.8: Control 1 */
#ifdef  LE_HOST
        uint32  c_vclabel:20,           /* HTLS VC Label / c_class_tag:16 */
                :12;
#else
        uint32  :12,
                c_vclabel:20;
#endif
                                        /* T6.9: Control 2 */
#ifdef  LE_HOST
        uint32  :6,
                c_opcode:3,             /* hdr opcode */
                c_dest_tgid:3,          /* dest trunk bit (if no classtag) */
                c_dest_trunk:1,         /* dest trunk (if no classtag) */
                c_mirror:1,             /* packet is mirrored */
                c_mirror_done:1,        /* mirroring done for packet */
                c_mirror_only:1,        /* mirror only */
                :16;
#else
        uint32  :16,
                c_mirror_only:1,
                c_mirror_done:1,
                c_mirror:1,
                c_dest_trunk:1,
                c_dest_tgid:3,
                c_opcode:3,
                :6;
#endif
                                        /* T6.10: Status 0 (same as T3) */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                crc:1,                  /* Pkt had invalid CRC */
                tagged:1,               /* Pkt was tagged */
                srcport_hi:1,           /* Source port [4] */
                l3:1,                   /* Pkt was l3 switched */
                start:1,                /* Start of pkt */
                cos:3,                  /* cpu class of service */
                srcport_lo:5,           /* Source Port [3:0] */
                error:1,                /* Error occurred */
                end:1,                  /* End of pkt */
                s0valid:1;              /* Status word valid */
#else
        uint32  s0valid:1,
                end:1,
                error:1,
                srcport_lo:5,
                cos:3,
                start:1,
                l3:1,
                srcport_hi:1,
                tagged:1,
                crc:1,
                count:16;
#endif
                                        /* T6.11: Status 1 (same as T3) */
#ifdef  LE_HOST
        uint32  ingport_lo:4,           /* local source (ingress) port [3:0] */
                opcode:3,               /* hdr opcode */
                prio_lo:1,              /* hdr priority [0] */
                s1valid:1,              /* status word valid */
                prio_hi:2,              /* hdr priority [2:1] */
                srcmod:5,               /* source module */
                reason:16;              /* cpu opcode */
#else
        uint32  reason:16,
                srcmod:5,
                prio_hi:2,
                s1valid:1,
                prio_lo:1,
                opcode:3,
                ingport_lo:4;
#endif
                                        /* T6.12: Status 2 (same as T3) */
#ifdef  LE_HOST
        uint32  class_tag:16,           /* classification / vc label [15:0] */
                match_rule:7,           /* match rule */
                destport:5,             /* destination port */
                destmod_lo:3,           /* destination module [2:0] */
                s2valid:1;              /* status word valid */
#else
        uint32  s2valid:1,
                destmod_lo:3,
                destport:5,
                match_rule:7,
                class_tag:16;
#endif
                                        /* T6.13: Status 3 */
#ifdef  LE_HOST
        uint32  ingport_hi:1,           /* local source (ingress) port [4] */
                match_rule_hi:1,        /* match rule [7] */
                destmod_hi:2,           /* dest module [4:3] */
                vclabel_hi:4,           /* vc label [19:16] */
                :23,
                s3valid:1;              /* status word valid */
#else
        uint32  s3valid:1,
                :23,
                vclabel_hi:4,
                destmod_hi:2,
                match_rule_hi:1,
                ingport_hi:1;
#endif
                                        /* T6.14: Status 4 */
#ifdef  LE_HOST
        uint32  s4purge:1,              /* purge */
                s4destmod1:1,           /* destination module is high */
                s4aged:1,               /* packet aged in mmu */
                s4untagged:1,           /* untagged packet */
                s4ipmc_repl:1,          /* ipmc replication start */
                s4destport:5,           /* destination port */
                s4smp:1,                /* switch on mirror port */
                s4l3_stats:1,           /* received for l3 stats */
                s4srcmod1:1,            /* source module is high */
                s4cos:3,                /* class of service */
                :1,
                s4hdr_type:2,           /* higig header type */
                s4cng:1,                /* congestion */
                :1,
                s4dest_tgid:3,          /* destination trunk group */
                s4dest_trunk:1,         /* destination is a trunk */
                s4mirror:1,             /* packet is mirrored */
                s4mirror_done:1,        /* mirroring done */
                s4mirror_only:1,        /* mirror only */
                s4pfm:2,                /* port filtering mode */
                :1,
                s4valid:1;              /* status word valid */
#else
    uint32      s4valid:1,
                :1,
                s4pfm:2,
                s4mirror_only:1,
                s4mirror_done:1,
                s4mirror:1,
                s4dest_trunk:1,
                s4dest_tgid:3,
                :1,
                s4cng:1,
                s4hdr_type:2,
                :1,
                s4cos:3,
                s4srcmod1:1,
                s4l3_stats:1,
                s4smp:1,
                s4destport:5,
                s4ipmc_repl:1,
                s4untagged:1,
                s4aged:1,
                s4destmod1:1,
                s4purge:1;
#endif
} dcb6_t;
#endif  /* BCM_TUCANA_SUPPORT */

#ifdef  BCM_DRACO15_SUPPORT
/*
 * DMA Control Block - Type 7
 * Used on 5695 devices
 * 10 words
 */
typedef struct {
        uint32  addr;                   /* T7.0: physical address */
                                        /* T7.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:15,             /* Requested byte count */
                c_crc:1,                /* send pkt asis or regen crc */
                c_l3:1,                 /* l3 switch packet in egress */
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                c_l3stat:1,             /* update l3 stats in egress */
                c_opcode:4,             /* hdr opcode */
                c_tagged:1,             /* tagged packet */
                c_prio:3,               /* hdr priority */
                c_cos:3,                /* class of service */
                c_chain:1;              /* Chaining */
#else
        uint32  c_chain:1,
                c_cos:3,
                c_prio:3,
                c_tagged:1,
                c_opcode:4,
                c_l3stat:1,
                c_sg:1,
                c_reload:1,
                c_l3:1,
                c_crc:1,
                c_count:15;
#endif  /* LE_HOST */
        uint32  l2ports;                /* T7.2: L2 transmit to ports */
        uint32  utports;                /* T7.3: L2 untagged ports */
        uint32  l3ports;                /* T7.4: L3 transmit to ports */
                                        /* T7.5: Control 1 */
#ifdef  LE_HOST
        uint32  c_destmod:7,            /* Destination module */
                c_destport:6,           /* Destination port */
                c_srcmod:7,             /* Source module */
                c_srcport:6,            /* Source port/trunk */
                c_pfm:2,                /* Port filtering mode */
                c_smp:1,                /* Switch on mirror port */
                :3;
#else
        uint32  :3,
                c_smp:1,
                c_pfm:2,
                c_srcport:6,
                c_srcmod:7,
                c_destport:6,
                c_destmod:7;
#endif
                                        /* T7.6: Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                crc:1,                  /* CRC Regen */
                                        /* DMA_GARBAGE_COLLECT set, pkt_aged */
                tagged:1,               /* Ingress tagged */
                l3:1,                   /* L3 switch info received */
                start:1,                /* S bit when DMA started */
                cos:3,                  /* CPU COS */
                error:1,                /* Packet error */
                end:1,                  /* Last/End packet */
                ingport:6,              /* packet ingress source port */
                cng:1;                  /* Packet marked for congestion */
#else
        uint32  cng:1,
                ingport:6,
                end:1,
                error:1,
                cos:3,
                start:1,
                l3:1,
                tagged:1,
                crc:1,
                count:16;
#endif
                                        /* T7.7: Status 1 */
#ifdef  LE_HOST
        uint32  srcport:6,              /* Source port/trunk */
                srcmod:7,               /* Source modid */
                destport:6,             /* Destination port / mcindex */
                destmod:7,              /* Destination module / mcindex */
                opcode:4,               /* hdr opcode */
                pfm:2;                  /* Port Filtering Mode */
#else
        uint32  pfm:2,
                opcode:4,
                destmod:7,
                destport:6,
                srcmod:7,
                srcport:6;
#endif
                                        /* T7.8: Status 2 */
#ifdef  LE_HOST
        uint32  class_tag:16,           /* classification tag */
                match_rule:16;          /* match rule */
#else
        uint32  match_rule:16,
                class_tag:16;
#endif
                                        /* T7.9: Status 3 */
#ifdef  LE_HOST
        uint32  reason:24,              /* CPU opcode */
                :2,
                hdr_type:2,             /* higig hdr type */
                prio:3,                 /* hdr priority */
                s3valid:1;              /* status word valid */
#else
        uint32  s3valid:1,
                prio:3,
                hdr_type:2,
                :2,
                reason:24;
#endif
} dcb7_t;
#endif  /* BCM_DRACO15_SUPPORT */


#if defined(BCM_XGS3_SWITCH_SUPPORT) || defined(BCM_PETRA_SUPPORT)
/*
 * DMA Control Block - Type 9
 * Used on 56504 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T9.0: physical address */
                                        /* T9.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T9.2: Module Header word 0 */
        uint32  mh1;                    /* T9.3: Module Header word 1 */
        uint32  mh2;                    /* T9.4: Module Header word 2 */
#ifdef  LE_HOST
                                        /* T9.5: RX Status 0 */
        uint32  mtp_index_hi:5,         /* Mirror-to-Port Index */
                l3uc:1,                 /* L3 UC */
                l3only:1,               /* L3 only */
                l3ipmc:1,               /* L3 IPMC */
                l3_intf:12,             /* L3 Intf number */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                cos:3,                  /* Packet Priority */
                cpu_cos:3,              /* CPU COS */
                chg_tos:1,              /* DSCP Changed */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1;              /* VLAN ID Added */
                                        /* T9.6: RX Status 1 */
        uint32  reason:25,              /* CPU opcode */
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                mtp_index_lo:2;         /* Mirror-to-Port Index */
                                        /* T9.7: RX Status 2 */
        uint32  dscp_hi:2,              /* New DSCP */
                srcport:5,              /* Source port */
                nh_index:13,            /* Next hop index */
                match_rule:10,          /* Matched Rule */
                match_rule_valid:1,     /* Matched Rule valid */
                decap_iptunnel:1;       /* Decap IP Tunneling Packet */

                                        /* T9.8: RX Status 3 */
        uint32  :11,
                ingress_untagged:1,     /* Pkt came in untagged */
                outer_vid:12,           /* VID */
                outer_cfi:1,            /* CFI */
                outer_pri:3,            /* Priority */
                dscp_lo:4;              /* New DSCP */
#else
                                        /* T9.5: RX Status 0 */
        uint32  add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                chg_tos:1,              /* DSCP Changed */
                cpu_cos:3,              /* CPU COS */
                cos:3,                  /* Packet Priority */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
                l3_intf:12,             /* L3 Intf number */
                l3ipmc:1,               /* L3 IPMC */
                l3only:1,               /* L3 only */
                l3uc:1,                 /* L3 UC */
                mtp_index_hi:5;         /* Mirror-to-Port Index */
                                        /* T9.6: RX Status 1 */
        uint32  mtp_index_lo:2,         /* Mirror-to-Port Index */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                reason:25;              /* CPU opcode */
                                        /* T9.7: RX Status 2 */
        uint32  decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                match_rule_valid:1,     /* Matched Rule valid */
                match_rule:10,          /* Matched Rule */
                nh_index:13,            /* Next hop index */
                srcport:5,              /* Source port */
                dscp_hi:2;              /* New DSCP */
                                        /* T9.8: RX Status 3 */
        uint32  dscp_lo:4,              /* New DSCP */
                outer_pri:3,            /* Priority (D)*/
                outer_cfi:1,            /* CFI (D)*/
                outer_vid:12,           /* VID (D)*/
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                :11;
#endif

        uint32  s_4;                    /* T9.9: RX Status 4 */
                                        /* T9.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb9_t;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef  BCM_EASYRIDER_SUPPORT
/*
 * DMA Control Block - Type 10
 * Used on 56601 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T10.0: physical address */
                                        /* T10.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T10.2: Module Header word 0 */
        uint32  mh1;                    /* T10.3: Module Header word 1 */
        uint32  mh2;                    /* T10.4: Module Header word 2 */
#ifdef  LE_HOST
                                        /* T10.5: RX Status 0 */
        uint32  mtp_index:5,            /* Mirror-to-Port Index (D)*/
                l3uc:1,                 /* L3 UC */
                l3only:1,               /* L3 only */
                l3ipmc:1,               /* L3 IPMC */
                l3_intf:12,             /* L3 Intf number */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                cos:3,                  /* Packet Priority */
                cpu_cos:3,              /* CPU COS */
                chg_tos:1,              /* DSCP Changed */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1;              /* VLAN ID Added */
                                        /* T10.6: RX Status 1 */
        uint32  reason_hi:26,           /* CPU opcode (D)*/
                pkt_not_changed:1,      /* L2 pkt unmodified for L2 MPLS (D)*/
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1;             /* Pkt is Aged */
                                        /* T10.8: RX Status 2 */
        uint32  nh_index_hi:14,         /* Next hop index (D)*/
                match_rule:9,           /* Matched Rule (D)*/
                decap_iptunnel:3,       /* Decap IP Tunneling Packet (D)*/
                reason_lo:6;            /* CPU opcode (D)*/
                                        /* T10.8: RX Status 3 */
        uint32  :18,
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                ecn:2,                  /* New ECN */
                dscp:6,                 /* New DSCP */
                srcport:4,              /* Source port (D)*/
                nh_index_lo:1;          /* Next hop index (D)*/
#else
                                        /* T10.5: RX Status 0 */
        uint32  add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                chg_tos:1,              /* DSCP Changed */
                cpu_cos:3,              /* CPU COS */
                cos:3,                  /* Packet Priority */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
                l3_intf:12,             /* L3 Intf number */
                l3ipmc:1,               /* L3 IPMC */
                l3only:1,               /* L3 only */
                l3uc:1,                 /* L3 UC */
                mtp_index:5;            /* Mirror-to-Port Index (D)*/
                                        /* T10.6: RX Status 1 */
        uint32  pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                pkt_not_changed:1,      /* L2 pkt unmodified for L2 MPLS (D)*/
                reason_hi:26;           /* CPU opcode (D)*/
                                        /* T10.8: RX Status 2 */
        uint32  reason_lo:6,            /* CPU opcode (D)*/
                decap_iptunnel:3,       /* Decap IP Tunneling Packet (D)*/
                match_rule:9,           /* Matched Rule (D)*/
                nh_index_hi:14;         /* Next hop index (D)*/
                                        /* T10.8: RX Status 3 */
        uint32  nh_index_lo:1,          /* Next hop index (D)*/
                srcport:4,              /* Source port (D)*/
                dscp:6,                 /* New DSCP */
                ecn:2,                  /* New ECN */
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                :18;
#endif

        uint32  s_4;                    /* T10.9: RX Status 4 */
                                        /* T10.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb10_t;
#endif  /* BCM_EASYRIDER_SUPPORT */

#ifdef  BCM_BRADLEY_SUPPORT
/*
 * DMA Control Block - Type 11
 * Used on 5680x, 5670x and 56580 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T11.0: physical address */
                                        /* T11.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                :12;                    /* Don't care */
#else
        uint32  :12,                    /* Don't care */
                c_hg:1,                 /* Higig (TX) */
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                c_chain:1,              /* Chaining */
                c_count:16;             /* Requested byte count */
#endif  /* LE_HOST */
        uint32  mh0;                    /* T11.2: Module Header word 0 */
        uint32  mh1;                    /* T11.3: Module Header word 1 */
        uint32  mh2;                    /* T11.4: Module Header word 2 */
        uint32  mh3;                    /* T11.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T11.6: RX Status 0 */
        uint32  cos:4,                  /* COS queue for non-CPU packets */
                cpu_cos:4,              /* COS queue for CPU packets */
                l3_intf:12,             /* Interface from IPMC replication */
                l3ipmc:1,               /* L3 IPMC replicated copy */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                chg_tos:1,              /* DSCP has been changed by HW */
                cell_error:1,           /* CRC error in buffer memory cell */
                bpdu:1,                 /* BPDU packet */
                add_vid:1,              /* Add VLAN ID */
                do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                hgi:2,                  /* Higig Interface Format Indicator */
                :1,                     /* Reserved */
                hg_type:1;              /* 0: Higig+, 1: Higig2 */

        uint32  reason:25,              /* CPU opcode */
                mtp_index:7;            /* Mirror-to port */

        uint32  outer_vid:12,           /* Outer VLAN ID */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_pri:3,            /* Outer priority */
                dscp:8,                 /* New DSCP */
                decap_iptunnel:1,       /* IP tunnel was decapsulated */
                srcport:5,              /* Source port number */
                ecn:2;                  /* New ECN */

        uint32  nh_index:13,            /* Next hop index */
                ingress_untagged:1,     /* Packet came in untagged */
                regen_crc:1,            /* Packet modified and needs new CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source port was Higig */
                purge_cell:1,           /* Packet contains purged cells */
                pkt_aged:1,             /* Packet aged while in MMU buffer */
                l3uc:1,                 /* L3 unicast packet */
                l3only:1,               /* L3 only IPMC packet */
                match_rule:10,          /* Matched FP rule */
                match_rule_valid:1;     /* Matched Rule valid */

                                        /* T11.10: DMA Status 0 */
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
                                        /* T11.6: RX Status 0 */
        uint32  hg_type:1,              /* 0: Higig+, 1: Higig2 */
                :1,                     /* Reserved */
                hgi:2,                  /* Higig Interface Format Indicator */
                do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                add_vid:1,              /* Add VLAN ID */
                bpdu:1,                 /* BPDU packet */
                cell_error:1,           /* CRC error in buffer memory cell */
                chg_tos:1,              /* DSCP has been changed by HW */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
                l3ipmc:1,               /* L3 IPMC replicated copy */
                l3_intf:12,             /* Interface from IPMC replication */
                cpu_cos:4,              /* COS queue for CPU packets */
                cos:4;                  /* COS queue for non-CPU packets */

        uint32  mtp_index:7,            /* Mirror-to port */
                reason:25;              /* CPU opcode */

        uint32  ecn:2,                  /* New ECN */
                srcport:5,              /* Source port number */
                decap_iptunnel:1,       /* IP tunnel was decapsulated */
                dscp:8,                 /* New DSCP */
                outer_pri:3,            /* Outer priority */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_vid:12;           /* Outer VLAN ID */

        uint32  match_rule_valid:1,     /* Matched Rule valid */
                match_rule:10,          /* Matched FP rule */
                l3only:1,               /* L3 only IPMC packet */
                l3uc:1,                 /* L3 unicast packet */
                pkt_aged:1,             /* Packet aged while in MMU buffer */
                purge_cell:1,           /* Packet contains purged cells */
                src_hg:1,               /* Source port was Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Packet modified and needs new CRC */
                ingress_untagged:1,     /* Packet came in untagged */
                nh_index:13;            /* Next hop index */

                                        /* T11.10: DMA Status 0 */
        uint32  done:1,                 /* Descriptor Done */
                dc:12,                  /* Don't Care */
                error:1,                /* Cell Error (RX) */
                start:1,                /* Start bit (RX) */
                end:1,                  /* End bit (RX) */
                count:16;               /* Transferred byte count */
#endif
} dcb11_t;
#endif  /* BCM_BRADLEY_SUPPORT */

#ifdef BCM_RAPTOR_SUPPORT
/*
 * DMA Control Block - Type 12
 * Used on 56218 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T12.0: physical address */
                                        /* T12.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* Reserved (TX) */
                c_pause:1,              /* Reserved (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T12.2: Module Header word 0 */
        uint32  mh1;                    /* T12.3: Module Header word 1 */
        uint32  mh2;                    /* T12.4: Module Header word 2 */
        uint32  mh3;                    /* T12.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T12.6: RX Status 0 */
        uint32  :6,                     /* Reserved */
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                mtp_index:7,            /* Mirror-to-Port Index */
                l3uc:1,                 /* L3 UC */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                cos:3,                  /* Packet Priority */
                cpu_cos:3,              /* CPU COS */
                chg_tos:1,              /* DSCP Changed */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1,              /* VLAN ID Added */
                do_not_change_ttl:1;    /* FP indicator for L3 unicast */

                                        /* T12.7: RX Status 0 */
        uint32  reason:26,              /* CPU opcode */
                :6;                     /* Padding */

                                        /* T12.8: RX Status 2 */
        uint32  dscp:8,                 /* New DSCP */
                srcport:6,              /* Source port */
                nh_index:8,             /* Next hop index */
                match_rule:10;          /* Matched Rule */

                                        /* T12.9: RX Status 3 */
        uint32  :14,
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                ingress_untagged:1,     /* Pkt came in untagged */
                outer_vid:12,           /* VID */
                outer_cfi:1,            /* CFI */
                outer_pri:3;            /* Priority */
#else
                                        /* T12.6: RX Status 0 */
        uint32  do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                chg_tos:1,              /* DSCP Changed */
                cpu_cos:3,              /* CPU COS */
                cos:3,                  /* Packet Priority */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
                l3uc:1,                 /* L3 UC */
                mtp_index:7,            /* Mirror-to-Port Index */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                :6;
                                        /* T12.7: RX Status 1 */
        uint32  :6,
                reason:26;              /* CPU opcode */
                                        /* T12.8: RX Status 2 */
        uint32  match_rule:10,          /* Matched Rule */
                nh_index:8,             /* Next hop index */
                srcport:6,              /* Source port */
                dscp:8;                 /* New DSCP */
                                        /* T12.9: RX Status 3 */
        uint32  outer_pri:3,            /* Priority (D)*/
                outer_cfi:1,            /* CFI (D)*/
                outer_vid:12,           /* VID (D)*/
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                :14;
#endif

                                        /* T12.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb12_t;
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
/*
 * DMA Control Block - Type 13
 * Used on 56514 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T13.0: physical address */
                                        /* T13.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T13.2: Module Header word 0 */
        uint32  mh1;                    /* T13.3: Module Header word 1 */
        uint32  mh2;                    /* T13.4: Module Header word 2 */
#ifdef  LE_HOST
                                        /* T13.5: RX Status 0 */
        uint32  mtp_index_hi:5,         /* Mirror-to-Port Index */
                l3uc:1,                 /* L3 UC */
                l3only:1,               /* L3 only */
                l3ipmc:1,               /* L3 IPMC */
                l3_intf:12,             /* L3 Intf number */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                cos:3,                  /* Packet Priority */
                cpu_cos:3,              /* CPU COS */
                chg_tos:1,              /* DSCP Changed */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1;              /* VLAN ID Added */
                                        /* T13.6: RX Status 1 */
        uint32  reason:25,              /* CPU opcode */
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                mtp_index_lo:2;         /* Mirror-to-Port Index */
                                        /* T13.7: RX Status 2 */
        uint32  dscp_hi:2,              /* New DSCP */
                srcport:5,              /* Source port */
                nh_index:13,            /* Next hop index */
                match_rule:10,          /* Matched Rule */
                match_rule_valid:1,     /* Matched Rule valid */
                decap_iptunnel:1;       /* Decap IP Tunneling Packet */

                                        /* T13.8: RX Status 3 */
        uint32  :8,
                inner_vlan_add:1,       /* Inner VLAN tag, only for Firebolt_2 */
                ingress_untagged:1,     /* Pkt came in untagged */
                outer_vid:12,           /* VID */
                outer_cfi:1,            /* CFI */
                outer_pri:3,            /* Priority */
                dscp_lo:6;              /* New DSCP */
#else
                                        /* T13.5: RX Status 0 */
        uint32  add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                chg_tos:1,              /* DSCP Changed */
                cpu_cos:3,              /* CPU COS */
                cos:3,                  /* Packet Priority */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
                l3_intf:12,             /* L3 Intf number */
                l3ipmc:1,               /* L3 IPMC */
                l3only:1,               /* L3 only */
                l3uc:1,                 /* L3 UC */
                mtp_index_hi:5;         /* Mirror-to-Port Index */
                                        /* T13.6: RX Status 1 */
        uint32  mtp_index_lo:2,         /* Mirror-to-Port Index */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                reason:25;              /* CPU opcode */
                                        /* T13.7: RX Status 2 */
        uint32  decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                match_rule_valid:1,     /* Matched Rule valid */
                match_rule:10,          /* Matched Rule */
                nh_index:13,            /* Next hop index */
                srcport:5,              /* Source port */
                dscp_hi:2;              /* New DSCP */
                                        /* T13.8: RX Status 3 */
        uint32  dscp_lo:6,              /* New DSCP */
                outer_pri:3,            /* Priority (D)*/
                outer_cfi:1,            /* CFI (D)*/
                outer_vid:12,           /* VID (D)*/
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                inner_vlan_add:1,       /* Inner VLAN tag, only for Firebolt_2 */
                :8;
#endif

        uint32  s_4;                    /* T13.9: RX Status 4 */
                                        /* T13.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb13_t;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_FIREBOLT_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
/*
 * DMA Control Block - Type 14
 * Used on 5661x devices
 * 16 words
 */
typedef struct {
        uint32  addr;                   /* T14.0: physical address */
                                        /* T14.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T14.2: Module Header word 0 */
        uint32  mh1;                    /* T14.3: Module Header word 1 */
        uint32  mh2;                    /* T14.4: Module Header word 2 */
        uint32  mh3;                    /* T14.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T14.6: RX Status 0 */
        uint32  :8,                     /* Reserved */
                cpu_cos:6,              /* COS queue for CPU packets */
                :2,                     /* Reserved */
                pkt_len:14,             /* Packet Length After Modification */
                :2;                     /* Reserved */

                                        /* T14.7 */ 
        uint32  reason_hi:16,           /* CPU opcode (high bits) */
                :16;                    /* Reserved */

                                        /* T14.8 */
        uint32  reason;                 /* CPU opcode */

                                        /* T14.9 */
        uint32  dscp:8,                 /* New DSCP */ 
                decap_iptunnel:1,       /* IP tunnel was decapsulated */
                chg_tos:1,              /* DSCP has been changed by HW */
                regen_crc:1,            /* Packet modified and needs new CRC */
                ingress_untagged:1,     /* Packet came in untagged */
                :4,                     /* Reserved */
                outer_vid:12,           /* Outer VLAN ID */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_pri:3;            /* Outer priority */

                                        /* T14.10 */
        uint32  reserved_w10;           /* Reserved */

                                        /* T14.11 */
        uint32  cos:3,                  /* COS */ 
                :5,                     /* Reserved */
                higig_cos:5,            /* Higig COS */
                :3,                     /* Reserved */
                vlan_cos:5,             /* VLAN COS */
                :3,                     /* Reserved */
                shaping_cos_sel:2,      /* Shaping COS Select */
                :6;                     /* Reserved */

                                        /* T14.12 */
        uint32  srcport:8,              /* Source port number */
                hgi:2,                  /* Higig Interface Format Indicator */
                itag_status:2,          /* Ingress incoming tag status */
                otag_action:2,          /* Ingress Outer tag action */
                itag_action:2,          /* Ingress Inner tag action */
                switch_pkt:1,           /* Switched packet */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                l3uc:1,                 /* L3 UC */
                l3only:1,               /* L3 only IPMC packet */
                l3ipmc:1,               /* L3 IPMC replicated copy */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                bpdu:1,                 /* BPDU Packet */
                :3;                     /* Reserved */

                                        /* T14.13 */
        uint32  nh_index:13,            /* Next hop index */
                :3,                     /* Reserved */
                em_mtp_index:5,         /* Egress Mirror-to port */
                :3,                     /* Reserved */
                mtp_index:6,            /* Ingress Mirror-to port */
                :2;                     /* Reserved */

                                        /* T14.14 */
        uint32  l3_intf:12,             /* L3 Intf number */
                :4,                     /* Reserved */
                match_rule:11,          /* Matched FP rule */
                :5;                     /* Reserved */

                                        /* T14.15: DMA Status 0 */
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
                                        /* T14.6: RX Status 0 */
        uint32  :2,                     /* Reserved */
                pkt_len:14,             /* Packet Length After Modification */
                :2,                     /* Reserved */
                cpu_cos:6,              /* COS queue for CPU packets */
                :8;                     /* Reserved */

                                        /* T14.7 */
        uint32  :16,                    /* CPU opcode (high bits) */
                reason_hi:16;           /* Reserved */

                                        /* T14.8 */
        uint32  reason;                 /* CPU opcode */

                                        /* T14.9 */
        uint32  outer_pri:3,            /* Outer priority */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_vid:12,           /* Outer VLAN ID */
                :4,                     /* Reserved */
                ingress_untagged:1,     /* Packet came in untagged */
                regen_crc:1,            /* Packet modified and needs new CRC */
                chg_tos:1,              /* DSCP has been changed by HW */
                decap_iptunnel:1,       /* IP tunnel was decapsulated */
                dscp:8;                 /* New DSCP */

                                        /* T14.10 */
        uint32  reserved_w10;           /* Reserved */

                                        /* T14.11 */
        uint32  :6,                     /* Reserved */
                shaping_cos_sel:2,      /* Shaping COS Select */
                :3,                     /* Reserved */
                vlan_cos:5,             /* VLAN COS */
                :3,                     /* Reserved */
                higig_cos:5,            /* Higig COS */
                :5,                     /* Reserved */
                cos:3;                  /* COS */

                                        /* T14.12 */
        uint32  :3,                     /* Reserved */
                bpdu:1,                 /* BPDU Packet */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
		l3ipmc:1,               /* L3 IPMC replicated copy */
		l3only:1,               /* L3 only IPMC packet */
                l3uc:1,                 /* L3 UC */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                switch_pkt:1,           /* Switched packet */
                itag_action:2,          /* Ingress Inner tag action */
                otag_action:2,          /* Ingress Outer tag action */
                itag_status:2,          /* Incoming tag status */
                hgi:2,                  /* Higig Interface Format Indicator */
                srcport:8;              /* Source port number */

                                        /* T14.13 */
        uint32  :2,                     /* Reserved */
                mtp_index:6,            /* Ingress Mirror-to port */
                :3,                     /* Reserved */
                em_mtp_index:5,         /* Egress Mirror-to port */
                :3,                     /* Reserved */
                nh_index:13;            /* Next hop index */

                                        /* T14.14 */
        uint32  :5,                     /* Reserved */
                match_rule:11,          /* Matched FP rule */
                :4,                     /* Reserved */
                l3_intf:12;             /* L3 Intf number */

                                        /* T14.15: DMA Status 0 */
        uint32  done:1,                 /* Descriptor Done */
                dc:12,                  /* Don't Care */
                error:1,                /* Cell Error (RX) */
                start:1,                /* Start bit (RX) */
                end:1,                  /* End bit (RX) */
                count:16;               /* Transferred byte count */
#endif
} dcb14_t;
#endif  /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_RAPTOR_SUPPORT
/*
 * DMA Control Block - Type 15
 * Used on 56224_A0 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T15.0: physical address */
                                        /* T15.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* Reserved (TX) */
                c_pause:1,              /* Reserved (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T15.2: Module Header word 0 */
        uint32  mh1;                    /* T15.3: Module Header word 1 */
        uint32  mh2;                    /* T15.4: Module Header word 2 */
        uint32  mh3;                    /* T15.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T15.6: RX Status 0 */
        uint32  l3_intf:12,             /* Interface from IPMC replication */
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                mtp_index:7,            /* Mirror-to-Port Index */
                l3uc:1,                 /* L3 UC */
		l3only:1,               /* L3 only IPMC packet */
		l3ipmc:1,               /* L3 IPMC replicated copy */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1,              /* VLAN ID Added */
                do_not_change_ttl:1;    /* FP indicator for L3 unicast */

                                        /* T15.7: RX Status 0 */
        uint32  reason:26,              /* CPU opcode */
                cos:3,                  /* Interface from IPMC replication */
                cpu_cos:3;              /* CPU COS */

                                        /* T15.8: RX Status 2 */
        uint32  dscp:8,                 /* New DSCP */
                nh_index:11,            /* Next hop index */
                match_rule:10,          /* Matched Rule */
                match_rule_valid:1,     /* Matched Rule valid */
                chg_tos:1,              /* DSCP changed */
                cell_error:1;           /* Cell CRC Checksum Error Detected */

                                        /* T15.9: RX Status 3 */
        uint32  :8,
		inner_vlan_add:1,       /* Inner VLAN tag added to packet */
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                ingress_untagged:1,     /* Pkt came in untagged */
                outer_vid:12,           /* VID */
                outer_cfi:1,            /* CFI */
                outer_pri:3,            /* Priority */
		srcport:5;		/* Source port */
#else
                                        /* T15.6: RX Status 0 */
        uint32  do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
		l3ipmc:1,               /* L3 IPMC replicated copy */
		l3only:1,               /* L3 only IPMC packet */
                l3uc:1,                 /* L3 UC */
                mtp_index:7,            /* Mirror-to-Port Index */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                l3_intf:12;             /* Interface from IPMC replication */
                                        /* T15.7: RX Status 1 */
        uint32  cpu_cos:3,              /* CPU COS */
                cos:3,                  /* Interface from IPMC replication */
                reason:26;              /* CPU opcode */
                                        /* T15.8: RX Status 2 */
        uint32  cell_error:1,           /* Cell CRC Checksum Error Detected */
                chg_tos:1,              /* DSCP changed */
                match_rule_valid:1,     /* Matched Rule valid */
		match_rule:10,          /* Matched Rule */
                nh_index:11,            /* Next hop index */
                dscp:8;                 /* New DSCP */
                                        /* T15.9: RX Status 3 */
        uint32  srcport:5,		/* Source port */
		outer_pri:3,            /* Priority (D)*/
                outer_cfi:1,            /* CFI (D)*/
                outer_vid:12,           /* VID (D)*/
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
		inner_vlan_add:1,       /* Inner VLAN tag added to packet */
                :8;
#endif

                                        /* T15.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb15_t;
#endif /* BCM_RAPTOR_SUPPORT */

#ifdef  BCM_SCORPION_SUPPORT
/*
 * DMA Control Block - Type 16
 * Used on 5661x devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T16.0: physical address */
                                        /* T16.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T16.2: Module Header word 0 */
        uint32  mh1;                    /* T16.3: Module Header word 1 */
        uint32  mh2;                    /* T16.4: Module Header word 2 */
        uint32  mh3;                    /* T16.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T16.6: RX Status 0 */
        uint32  cos:4,                  /* COS queue for non-CPU packets */
                :4,                     /* Reserved */
                cpu_cos:5,              /* COS queue for CPU packets */
                :3,                     /* Reserved */
                pkt_len:14,             /* Packet Length After Modification */
                :2;                     /* Reserved */

                                        /* T16.7 */ 
        uint32  reserved_w7;            /* Reserved */

                                        /* T16.8 */
        uint32  reason:32;              /* CPU opcode */

                                        /* T16.9 */
        uint32  dscp:8,                 /* New DSCP */ 
                decap_iptunnel:1,       /* IP tunnel was decapsulated */
                chg_tos:1,              /* DSCP has been changed by HW */
                regen_crc:1,            /* Packet modified and needs new CRC */
                ingress_untagged:1,     /* Packet came in untagged */
                :4,                     /* Reserved */
                outer_vid:12,           /* Outer VLAN ID */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_pri:3;            /* Outer priority */

                                        /* T16.10 */
        uint32  reserved_w10;           /* Reserved */

                                        /* T16.11 */
        uint32  reserved_w11;           /* Reserved */

                                        /* T16.12 */
        uint32  srcport:8,              /* Source port number */
                hgi:2,                  /* Higig Interface Format Indicator */
                itag_status:2,          /* Ingress incoming tag status */
                otag_action:2,          /* Ingress Outer tag action */
                itag_action:2,          /* Ingress Inner tag action */
                switch_pkt:1,           /* Switched packet */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                l3uc:1,                 /* L3 UC */
                l3only:1,               /* L3 only IPMC packet */
                l3ipmc:1,               /* L3 IPMC replicated copy */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                bpdu:1,                 /* BPDU Packet */
                :3;                     /* Reserved */
                                        /* T16.13 */
        uint32  nh_index:13,            /* Next hop index */
                :3,                     /* Reserved */
                em_mtp_index:5,         /* Egress Mirror-to port */
                :3,                     /* Reserved */
                mtp_index:6,            /* Ingress Mirror-to port */
                :2;                     /* Reserved */

                                        /* T16.14 */
        uint32  l3_intf:12,             /* L3 Intf number */
                :4,                     /* Reserved */
                match_rule:8,           /* Matched FP rule */
                :8;                     /* Reserved */

                                        /* T16.15: DMA Status 0 */
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
                                        /* T16.6: RX Status 0 */
        uint32  :2,                     /* Reserved */
                pkt_len:14,             /* Packet Length After Modification */
                :3,                     /* Reserved */
                cpu_cos:5,              /* COS queue for CPU packets */
                :4,                     /* Reserved */
                cos:4;                  /* COS queue for non-CPU packets */

                                        /* T16.7 */
        uint32  reserved_w7;            /* Reserved */

                                        /* T16.8 */
        uint32  reason:32;              /* CPU opcode */

                                        /* T16.9 */
        uint32  outer_pri:3,            /* Outer priority */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_vid:12,           /* Outer VLAN ID */
                :4,                     /* Reserved */
                ingress_untagged:1,     /* Packet came in untagged */
                regen_crc:1,            /* Packet modified and needs new CRC */
                chg_tos:1,              /* DSCP has been changed by HW */
                decap_iptunnel:1,       /* IP tunnel was decapsulated */
                dscp:8;                 /* New DSCP */

                                        /* T16.10 */
        uint32  reserved_w10;           /* Reserved */

                                        /* T16.11 */
        uint32  reserved_w11;           /* Reserved */

                                        /* T16.12 */
        uint32  :3,                     /* Reserved */
                bpdu:1,                 /* BPDU Packet */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
		l3ipmc:1,               /* L3 IPMC replicated copy */
		l3only:1,               /* L3 only IPMC packet */
                l3uc:1,                 /* L3 UC */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                switch_pkt:1,           /* Switched packet */
                itag_action:2,          /* Ingress Inner tag action */
                otag_action:2,          /* Ingress Outer tag action */
                itag_status:2,          /* Ingress incoming tag status */
                hgi:2,                  /* Higig Interface Format Indicator */
                srcport:8;              /* Source port number */

                                        /* T16.13 */
        uint32  :2,                     /* Reserved */
                mtp_index:6,            /* Ingress Mirror-to port */
                :3,                     /* Reserved */
                em_mtp_index:5,         /* Egress Mirror-to port */
                :3,                     /* Reserved */
                nh_index:13;            /* Next hop index */

                                        /* T16.14 */
        uint32  :8,                     /* Reserved */
                match_rule:8,           /* Matched FP rule */
                :4,                     /* Reserved */
                l3_intf:12;             /* L3 Intf number */

                                        /* T16.15: DMA Status 0 */
        uint32  done:1,                 /* Descriptor Done */
                dc:12,                  /* Don't Care */
                error:1,                /* Cell Error (RX) */
                start:1,                /* Start bit (RX) */
                end:1,                  /* End bit (RX) */
                count:16;               /* Transferred byte count */
#endif
} dcb16_t;
#endif  /* BCM_SCORPION_SUPPORT */

#ifdef BCM_HAWKEYE_SUPPORT
/*
 * DMA Control Block - Type 17
 * Used on 53314 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T17.0: physical address */
                                        /* T17.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* Reserved (TX) */
                c_pause:1,              /* Reserved (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* Only for PBSMH word 0 */
        uint32  mh1;                    /* Only for PBSMH word 1 */
        uint32  mh2;                    /* Only for PBSMH word 2 */
        uint32  mh3;                    /* Used for PBSMH word 3 and Timestamp for EAV packets in rx */
#ifdef  LE_HOST
                                        /* T17.6: RX Status 0 */
        uint32  l3_intf:12,             /* Interface from IPMC replication */
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                mtp_index:7,            /* Mirror-to-Port Index */
                l3uc:1,                 /* L3 UC */
		l3only:1,               /* L3 only IPMC packet */
		l3ipmc:1,               /* L3 IPMC replicated copy */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1,              /* VLAN ID Added */
                do_not_change_ttl:1;    /* FP indicator for L3 unicast */

                                        /* T17.7: RX Status 0 */
        uint32  reason:28,              /* CPU opcode */
                reserved_0:1,           /* Reserved */
                cpu_cos:3;              /* CPU COS */

                                        /* T17.8: RX Status 2 */
        uint32  dscp:8,                 /* New DSCP */
                nh_index:11,            /* Next hop index */
                match_rule:10,          /* Matched Rule */
                match_rule_valid:1,     /* Matched Rule valid */
                chg_tos:1,              /* DSCP changed */
                cell_error:1;           /* Cell CRC Checksum Error Detected */

                                        /* T17.9: RX Status 3 */
        uint32  :8,
		inner_vlan_add:1,       /* Inner VLAN tag added to packet */
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                ingress_untagged:1,     /* Pkt came in untagged */
                outer_vid:12,           /* VID */
                outer_cfi:1,            /* CFI */
                outer_pri:3,            /* Priority */
		srcport:5;		/* Source port */
#else
                                        /* T17.6: RX Status 0 */
        uint32  do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
		l3ipmc:1,               /* L3 IPMC replicated copy */
		l3only:1,               /* L3 only IPMC packet */
                l3uc:1,                 /* L3 UC */
                mtp_index:7,            /* Mirror-to-Port Index */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                l3_intf:12;             /* Interface from IPMC replication */
                                        /* T17.7: RX Status 1 */
        uint32  cpu_cos:3,              /* CPU COS */
                reserved_0:1,           /* Reserved */
                reason:28;              /* CPU opcode */
                                        /* T17.8: RX Status 2 */
        uint32  cell_error:1,           /* Cell CRC Checksum Error Detected */
                chg_tos:1,              /* DSCP changed */
                match_rule_valid:1,     /* Matched Rule valid */
		match_rule:10,          /* Matched Rule */
                nh_index:11,            /* Next hop index */
                dscp:8;                 /* New DSCP */
                                        /* T17.9: RX Status 3 */
        uint32  srcport:5,		/* Source port */
		outer_pri:3,            /* Priority (D)*/
                outer_cfi:1,            /* CFI (D)*/
                outer_vid:12,           /* VID (D)*/
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
		inner_vlan_add:1,       /* Inner VLAN tag added to packet */
                :8;
#endif

                                        /* T17.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb17_t;
#endif /* BCM_HAWKEYE_SUPPORT */

#ifdef BCM_RAPTOR_SUPPORT
/*
 * DMA Control Block - Type 18
 * Used on 56224_B0 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T18.0: physical address */
                                        /* T18.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* Reserved (TX) */
                c_pause:1,              /* Reserved (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T18.2: Module Header word 0 */
        uint32  mh1;                    /* T18.3: Module Header word 1 */
        uint32  mh2;                    /* T18.4: Module Header word 2 */
        uint32  mh3;                    /* T18.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T18.6: RX Status 0 */
        uint32  l3_intf:12,             /* Interface from IPMC replication */
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                mtp_index:7,            /* Mirror-to-Port Index */
                l3uc:1,                 /* L3 UC */
		l3only:1,               /* L3 only IPMC packet */
		l3ipmc:1,               /* L3 IPMC replicated copy */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1,              /* VLAN ID Added */
                do_not_change_ttl:1;    /* FP indicator for L3 unicast */

                                        /* T18.7: RX Status 0 */
        uint32  reason:26,              /* CPU opcode */
                cos:3,                  /* Interface from IPMC replication */
                cpu_cos:3;              /* CPU COS */

                                        /* T18.8: RX Status 2 */
        uint32  dscp:8,                 /* New DSCP */
                nh_index:12,            /* Next hop index */
                match_rule:10,          /* Matched Rule */
                match_rule_valid:1,     /* Matched Rule valid */
                cell_error:1;           /* Cell CRC Checksum Error Detected */

                                        /* T18.9: RX Status 3 */
        uint32  :7,
                chg_tos:1,              /* DSCP changed */
		inner_vlan_add:1,       /* Inner VLAN tag added to packet */
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                ingress_untagged:1,     /* Pkt came in untagged */
                outer_vid:12,           /* VID */
                outer_cfi:1,            /* CFI */
                outer_pri:3,            /* Priority */
		srcport:5;		/* Source port */
#else
                                        /* T18.6: RX Status 0 */
        uint32  do_not_change_ttl:1,    /* FP indicator for L3 unicast */
                add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
		l3ipmc:1,               /* L3 IPMC replicated copy */
		l3only:1,               /* L3 only IPMC packet */
                l3uc:1,                 /* L3 UC */
                mtp_index:7,            /* Mirror-to-Port Index */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                l3_intf:12;             /* Interface from IPMC replication */
                                        /* T18.7: RX Status 1 */
        uint32  cpu_cos:3,              /* CPU COS */
                cos:3,                  /* Interface from IPMC replication */
                reason:26;              /* CPU opcode */
                                        /* T18.8: RX Status 2 */
        uint32  cell_error:1,           /* Cell CRC Checksum Error Detected */
                match_rule_valid:1,     /* Matched Rule valid */
		match_rule:10,          /* Matched Rule */
                nh_index:12,            /* Next hop index */
                dscp:8;                 /* New DSCP */
                                        /* T18.9: RX Status 3 */
        uint32  srcport:5,		/* Source port */
		outer_pri:3,            /* Priority (D)*/
                outer_cfi:1,            /* CFI (D)*/
                outer_vid:12,           /* VID (D)*/
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                decap_iptunnel:1,       /* Decap IP Tunneling Packet */
		inner_vlan_add:1,       /* Inner VLAN tag added to packet */
                chg_tos:1,              /* DSCP changed */
                :7;
#endif

                                        /* T18.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb18_t;
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
/*
 * DMA Control Block - Type 19
 * Used on 5663x devices
 * 16 words
 */
#include <soc/shared/dcbformats/type19.h>
#endif  /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_ENDURO_SUPPORT)
/*
 * DMA Control Block - Type 20
 * Used on 5623x devices
 * 16 words
 */
typedef struct {
        uint32  addr;                   /* T20.0: physical address */
                                        /* T20.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T20.2: Module Header word 0 */
        uint32  mh1;                    /* T20.3: Module Header word 1 */
        uint32  mh2;                    /* T20.4: Module Header word 2 */
        uint32  mh3;                    /* T20.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T20.6: RX Status 0 */
        uint32  timestamp_type:2,       /* TIMESTAMP field type indicator */
                em_mtp_index:5,         /* EM Mirror-to-Port Index */
                :1,                     /* Reserved */
                cpu_cos:6,              /* COS queue for CPU packets */
                :2,                     /* Reserved */
                inner_vid:12,           /* Inner VLAN ID */
                inner_cfi:1,            /* Inner Canoncial Format Indicator */
                inner_pri:3;            /* Inner priority */

                                        /* T20.7 */ 
        uint32  reason_hi:16,           /* CPU opcode (high bits) */
                pkt_len:14,             /* Packet Length After Modification */
                :2;                     /* Reserved */

                                        /* T20.8 */
        uint32  reason;                 /* CPU opcode */

                                        /* T20.9 */
        uint32  dscp:8,                 /* New DSCP */
                chg_tos:1,              /* DSCP has been changed by HW */
                decap_tunnel_type:4,    /* Tunnel type that was decapsulated */
                regen_crc:1,            /* Packet modified and needs new CRC */
                :2,                     /* Reserved */
                outer_vid:12,           /* Outer VLAN ID */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_pri:3;            /* Outer priority */

                                        /* T20.10 */
        uint32  timestamp;              /* Timestamp */

                                        /* T20.11 */
        uint32  cos:4,                  /* COS */
                higig_cos:5,            /* Higig COS */
                vlan_cos:5,             /* VLAN COS */
                shaping_cos_sel:2,      /* Shaping COS Select */
                vfi:12,                 /* Internal VFI or FID value */
                vfi_valid:1,            /* VFI or FID */
                :3;                     /* Reserved */

                                        /* T20.12 */
        uint32  srcport:8,              /* Source port number */
                hgi:2,                  /* Higig Interface Format Indicator */
                itag_status:2,          /* Ingress incoming tag status */
                otag_action:2,          /* Ingress Outer tag action */
                itag_action:2,          /* Ingress Inner tag action */
                service_tag:1,          /* SD tag present */
                switch_pkt:1,           /* Switched packet */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                src_hg:1,               /* Source is Higig */
                l3routed:1,             /* Any IP routed packet */
                l3only:1,               /* L3 only IPMC packet */
                replicated:1,           /* Replicated copy */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                do_not_change_ttl:1,    /* Do not change TTL */
                bpdu:1,                 /* BPDU Packet */
                hg_lookup:1,            /* Reserved for future use */
                :4;                     /* Reserved */

                                        /* T20.13 */
        uint32  nhop_index:15,          /* L3 IP Multicast Interface Number or Next HOP Index */
                :1,                     /* Reserved */
                match_rule:8,           /* Matched FP rule */
                im_mtp_index:5,         /* IM Mirror-to-Port Index */
                ecn:2,                  /* Reserved */
                change_ecn:1;           /* Reserved */

                                        /* T20.14 */
        uint32  timestamp_upper;        /* OAM delay measurement: Upper 32-bit of 64-bit timestamp */

                                        /* T20.15: DMA Status 0 */
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
                                        /* T20.6: RX Status 0 */
        uint32  inner_pri:3,            /* Inner priority */
                inner_cfi:1,            /* Inner Canoncial Format Indicator */
                inner_vid:12,           /* Inner VLAN ID */
                :2,                     /* Reserved */
                cpu_cos:6,              /* COS queue for CPU packets */
                :1,                     /* Reserved */
                em_mtp_index:5,         /* EM Mirror-to-Port Index */
                timestamp_type:2;       /* TIMESTAMP field type indicator */

                                        /* T20.7 */
        uint32  :2,                     /* Reserved */
                pkt_len:14,             /* Packet Length After Modification */
                reason_hi:16;           /* CPU opcode (high bits) */

                                        /* T20.8 */
        uint32  reason;                 /* CPU opcode */

                                        /* T20.9 */
        uint32  outer_pri:3,            /* Outer priority */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_vid:12,           /* Outer VLAN ID */
                :2,                     /* Reserved */
                regen_crc:1,            /* Packet modified and needs new CRC */
                decap_tunnel_type:4,    /* Tunnel type that was decapsulated */
                chg_tos:1,              /* DSCP has been changed by HW */
                dscp:8;                 /* New DSCP */

                                        /* T20.10 */
        uint32  timestamp;              /* Timestamp */

                                        /* T20.11 */
        uint32  :3,                     /* Reserved */
                vfi_valid:1,            /* VFI or FID */
                vfi:12,                 /* Internal VFI or FID value */
                shaping_cos_sel:2,      /* Shaping COS Select */
                vlan_cos:5,             /* VLAN COS */
                higig_cos:5,            /* Higig COS */
                cos:4;                  /* COS */

                                        /* T20.12 */
        uint32  :4,                     /* Reserved */
                hg_lookup:1,            /* Reserved for future use */
                bpdu:1,                 /* BPDU Packet */
                do_not_change_ttl:1,    /* Do not change TTL */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
                replicated:1,           /* Replicated copy */
                l3only:1,               /* L3 only IPMC packet */
                l3routed:1,             /* Any IP routed packet */
                src_hg:1,               /* Source is Higig */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                switch_pkt:1,           /* Switched packet */
                service_tag:1,          /* SD tag present */
                itag_action:2,          /* Ingress Inner tag action */
                otag_action:2,          /* Ingress Outer tag action */
                itag_status:2,          /* Ingress incoming tag status */
                hgi:2,                  /* Higig Interface Format Indicator */
                srcport:8;              /* Source port number */

                                        /* T20.13 */
        uint32  change_ecn:1,           /* Reserved */
                ecn:2,                  /* Reserved */
                im_mtp_index:5,         /* IM Mirror-to-Port Index */
                match_rule:8,           /* Matched FP rule */
                :1,                     /* Reserved */
                nhop_index:15;          /* L3 IP Multicast Interface Number or Next HOP Index */

                                        /* T20.14 */
        uint32  timestamp_upper;        /* OAM delay measurement: Upper 32-bit of 64-bit timestamp */

                                        /* T20.15: DMA Status 0 */
        uint32  done:1,                 /* Descriptor Done */
                dc:12,                  /* Don't Care */
                error:1,                /* Cell Error (RX) */
                start:1,                /* Start bit (RX) */
                end:1,                  /* End bit (RX) */
                count:16;               /* Transferred byte count */
#endif
} dcb20_t;
#endif  /* BCM_ENDURO_SUPPORT */

#if defined(BCM_TRIDENT_SUPPORT)
/*
 * DMA Control Block - Type 21
 * Used on 56840 devices
 * 16 words
 */
typedef struct {
        uint32  addr;                   /* T21.0: physical address */
                                        /* T21.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* Update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_stat:1,               /* Update stats (TX) */
                c_hg:1,                 /* Higig (TX) */
                c_reload:1,             /* Reload */
                c_sg:1,                 /* Scatter Gather */
                c_chain:1,              /* Chaining */
                c_count:16;             /* Requested byte count */
#endif  /* LE_HOST */
        uint32  mh0;                    /* T21.2: Module Header word 0 */
        uint32  mh1;                    /* T21.3: Module Header word 1 */
        uint32  mh2;                    /* T21.4: Module Header word 2 */
        uint32  mh3;                    /* T21.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T21.6 */
        uint32  :3,                     /* Reserved */
                mtp_index:5,            /* MTP index */
                cpu_cos:8,              /* Queue number for CPU packets */
                inner_vid:12,           /* Inner VLAN ID */
                inner_cfi:1,            /* Inner Canoncial Format Indicator */
                inner_pri:3;            /* Inner priority */

                                        /* T21.7 */
        uint32  reason_hi:16,           /* CPU opcode (high bits) */
                pkt_len:14,             /* Packet length */
                :2;                     /* Reserved */

                                        /* T21.8 */
        uint32  reason;                 /* CPU opcode (low bits) */

                                        /* T21.9 */
        uint32  dscp:6,                 /* New DSCP */
                :2,                     /* Reserved */
                chg_tos:1,              /* DSCP has been changed by HW */
                decap_tunnel_type:4,    /* Tunnel type that was decapsulated */
                regen_crc:1,            /* Packet modified and needs new CRC */
                :2,                     /* Reserved */
                outer_vid:12,           /* Outer VLAN ID */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_pri:3;            /* Outer priority */

                                        /* T21.10 */
        uint32  timestamp;              /* Timestamp */

                                        /* T21.11 */
        uint32  :12,                    /* Reserved */
                ecn:2,                  /* New ECN value */
                chg_ecn:1,              /* ECN changed */
                mcq:1,                  /* Multicast queue indicator */
                vfi:12,                 /* Internal VFI value */
                vfi_valid:1,            /* Validates VFI field */
                dvp_nhi_sel:1,          /* DVP overlay indicator */
                vntag_action:2;         /* VN tag action */

                                        /* T21.12 */
        uint32  srcport:8,              /* Source port number */
                hgi:2,                  /* Higig Interface Format Indicator */
                itag_status:2,          /* Ingress Inner tag status */
                otag_action:2,          /* Ingress Outer tag action */
                itag_action:2,          /* Ingress Inner tag action */
                service_tag:1,          /* SD tag present */
                switch_pkt:1,           /* Switched packet */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                src_hg:1,               /* Source is Higig */
                l3routed:1,             /* Any IP routed packet */
                l3only:1,               /* L3 only IPMC packet */
                replicated:1,           /* Replicated copy */
                :2,                     /* Reserved */
                do_not_change_ttl:1,    /* Do not change TTL */
                bpdu:1,                 /* BPDU Packet */
                :5;                     /* Reserved */

                                        /* T21.13 */
        uint32  uc_cos:4,               /* COS value for unicast packet */
                eh_seg_sel:3,           /* Extended Higig Seg Sel*/
                next_pass_cp:2,         /* Next pass code point */
                oam_pkt:1,              /* OAM packet */
                :2,                     /* Reserved */
                eh_tag_type:2,          /* Extended Higig tag type */
                eh_tm:1,                /* Extended Higig traffic mgr ctrl */
                eh_queue_tag:16,        /* Extended Higig queue tag */
                hg2_ext_hdr:1;          /* Extended Higig2 header valid */

                                        /* T21.14 */
        uint32  l3_intf:15,             /* L3 Intf num / Next hop idx */
                all_switch_drop:1,      /* All switched copies dropped */
                match_rule:8,           /* Matched FP rule */
                mc_cos2:4,              /* COS2 value for multicast packets */
                mc_cos1:4;              /* COS1 value for multicast packets */

                                        /* T21.15: DMA Status 0 */
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                :12,                    /* Reserved */
                done:1;                 /* Descriptor Done */
#else
                                        /* T21.6 */
        uint32  inner_pri:3,            /* Inner priority */
                inner_cfi:1,            /* Inner Canoncial Format Indicator */
                inner_vid:12,           /* Inner VLAN ID */
                cpu_cos:8,              /* Queue number for CPU packets */
                mtp_index:5,            /* MTP index */
                :3;                     /* Reserved */

                                        /* T21.7 */
        uint32  :2,                     /* Reserved */
                pkt_len:14,             /* Packet length */
                reason_hi:16;           /* CPU opcode (high bits) */

                                        /* T21.8 */
        uint32  reason;                 /* CPU opcode (low bits) */

                                        /* T21.9 */
        uint32  outer_pri:3,            /* Outer priority */
                outer_cfi:1,            /* Outer Canoncial Format Indicator */
                outer_vid:12,           /* Outer VLAN ID */
                :2,                     /* Reserved */
                regen_crc:1,            /* Packet modified and needs new CRC */
                decap_tunnel_type:4,    /* Tunnel type that was decapsulated */
                chg_tos:1,              /* DSCP has been changed by HW */
                :2,                     /* Reserved */
                dscp:6;                 /* New DSCP */

                                        /* T21.10 */
        uint32  timestamp;              /* Timestamp */

                                        /* T21.11 */
        uint32  vntag_action:2,         /* VN tag action */
                dvp_nhi_sel:1,          /* DVP overlay indicator */
                vfi_valid:1,            /* Validates VFI field */
                vfi:12,                 /* Internal VFI value */
                mcq:1,                  /* Multicast queue indicator */
                chg_ecn:1,              /* ECN changed */
                ecn:2,                  /* New ECN value */
                :12,                    /* Reserved */

                                        /* T21.12 */
        uint32  :5,                     /* Reserved */
                bpdu:1,                 /* BPDU Packet */
                do_not_change_ttl:1,    /* Do not change TTL */
                :2,                     /* Reserved */
                replicated:1,           /* Replicated copy */
                l3only:1,               /* L3 only IPMC packet */
                l3routed:1,             /* Any IP routed packet */
                src_hg:1,               /* Source is Higig */
                hg_type:1,              /* 0: Higig+, 1: Higig2 */
                switch_pkt:1,           /* Switched packet */
                service_tag:1,          /* SD tag present */
                itag_action:2,          /* Ingress Inner tag action */
                otag_action:2,          /* Ingress Outer tag action */
                itag_status:2,          /* Ingress Inner tag status */
                hgi:2,                  /* Higig Interface Format Indicator */
                srcport:8;              /* Source port number */

                                        /* T21.13 */
        uint32  hg2_ext_hdr:1,          /* Extended Higig2 header valid */
                eh_queue_tag:16,        /* Extended Higig queue tag */
                eh_tm:1,                /* Extended Higig traffic mgr ctrl */
                eh_tag_type:2,          /* Extended Higig tag type */
                :2,                     /* Reserved */
                oam_pkt:1,              /* OAM packet */
                next_pass_cp:2,         /* Next pass code point */
                eh_seg_sel:3,           /* Extended Higig Seg Sel*/
                uc_cos:4;               /* COS value for unicast packet */

                                        /* T21.14 */
        uint32  mc_cos1:4,              /* COS1 value for multicast packets */
                mc_cos2:4,              /* COS2 value for multicast packets */
                match_rule:8,           /* Matched FP rule */
                all_switch_drop:1,      /* All switched copies dropped */
                l3_intf:15;             /* L3 Intf num / Next hop idx */

                                        /* T21.15: DMA Status 0 */
        uint32  done:1,                 /* Descriptor Done */
                :12,                    /* Reserved */
                error:1,                /* Cell Error (RX) */
                start:1,                /* Start bit (RX) */
                end:1,                  /* End bit (RX) */
                count:16;               /* Transferred byte count */
#endif
} dcb21_t;
#endif  /* BCM_TRIDENT_SUPPORT */

#ifdef  BCM_SHADOW_SUPPORT
/*
 * DMA Control Block - Type 22
 * Used on 88732 devices
 * 11 words
 */
typedef struct {
        uint32  addr;                   /* T22.0: physical address */
                                        /* T22.1: Control 0 */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T22.2: Module Header word 0 */
        uint32  mh1;                    /* T22.3: Module Header word 1 */
        uint32  mh2;                    /* T22.4: Module Header word 2 */
        uint32  mh3;                    /* T22.5: Module Header word 3 */
#ifdef  LE_HOST
                                        /* T22.6: RX Status 0 */
        uint32  :6,                     /* Reserved */
                dscp:8,                 /* DSCP */
                chg_tos:1,              /* DSCP changed */
                regen_crc:1,            /* CRC to be regenerated */
                ingress_untagged:1,     /* Packet came in untagged */
                :2,                     /* Reserved */
                src_port_mode:3,        /* Source port mode */
                :10;                    /* Reserved */

                                        /* T22.7 */ 
        uint32  reason:16,              /* CPU opcode */
                pkt_len:14,             /* Packet length */
                :2;                     /* Reserved */

                                        /* T22.8 */
        uint32  :8,                     /* Reserved */
                outer_vid:12,           /* Outer VID (EP) */
                outer_cfi:1,            /* Outer CFI (EP) */
                outer_pri:3,            /* Outer pri (EP) */
                match_rule:8;           /* Matched rule */

                                        /* T22.9 */
        uint32  :19,                    /* Reserved */
                msec_no_error:1,        /* MACSEC no error */
                msec_replay_error:1,    /* MACSEC replay error */
                mirror:1,               /* Mirrored copy */
                bpdu:1,                 /* BPDU packet */
                :1,                     /* Reserved */
                pkt_di_match:1,         /* Packet Dindex match */
                stripped_bytes:6,       /* MACSEC stripped bytes */
                msec_icv_fail:1;        /* MACSEC ICV fail */

                                        /* T22.10 */
        uint32  timestamp_hi:24,        /* Timestamp (high) */
                :8;                     /* Reserved */

                                        /* T22.11 */
        uint32  outer_tag_hi:13,        /* OTAG (high) */
                :11,                    /* Reserved */
                timestamp_lo:8;         /* Timestamp (lo) */

                                        /* T22.12 */
        uint32  srcport:5,              /* Source port number */
                :3,                     /* Reserved */
                msec_decap_status:2,    /* MACSEC decap status */
                unmodified:1,           /* Packet was unmodified */
                vlan_chk_disable:1,     /* VLAN checks were disabled */
                mpls_exp:3,             /* MPLS EXP */
                dei:1,                  /* DEI */
                sci_index:10,           /* SCI index */
                sci_valid:1,            /* SCI index valid */
                otag_action:2,          /* Ingress Outer tag action */
                outer_tag_lo:3;         /* OTAG (low) */

                                        /* T22.13 */
        uint32  mpls_ptr:8,             /* Pointer to first MPLS label */
                num_labels:2,           /* Number of MPLS labels */
                l3_ptr:8,               /* Pointer to L3 header */
                l3_type:3,              /* L3 protocol */
                l4_ptr:8,               /* Pointer to L4 header */
                l4_type:2,              /* L4 protocol */
                time_req:1;             /* Timestamping required by destport */

                                        /* T22.14 */
        uint32  :1,                     /* Reserved */
                src_hg:1,               /* Source is Higig */
                cng:2,                  /* Congestion */
                ecn:2,                  /* ECN */
                change_ecn:1,           /* ECN change */
                cell_error:1,           /* Cell error */
                destport:5,             /* Destination port */
                etype_ptr:7,            /* Ethertype pointer */
                frame_type:3,           /* Frame type */
                dot1q_ptr:7,            /* Pointer to VLAN tag(s) */
                num_tags:2;             /* Number of tags */

                                        /* T22.15: DMA Status 0 */
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
                                        /* T22.6: RX Status 0 */
        uint32  :10,                    /* Reserved */
                src_port_mode:3,        /* Source port mode */
                :2,                     /* Reserved */
                ingress_untagged:1,     /* Packet came in untagged */
                regen_crc:1,            /* CRC to be regenerated */
                chg_tos:1,              /* DSCP changed */
                dscp:8,                 /* DSCP */
                :6;                     /* Reserved */

                                        /* T22.7 */
        uint32  :2,                     /* Reserved */
                pkt_len:14,             /* Packet length */
                reason:16;              /* CPU opcode */

                                        /* T22.8 */
        uint32  match_rule:8,           /* Matched rule */
                outer_pri:3,            /* Outer pri (EP) */
                outer_cfi:1,            /* Outer CFI (EP) */
                outer_vid:12,           /* Outer VID (EP) */
                :8;                     /* Reserved */

                                        /* T22.9 */
        uint32  msec_icv_fail:1,        /* MACSEC ICV fail */
                stripped_bytes:6,       /* MACSEC stripped bytes */
                pkt_di_match:1,         /* Packet Dindex match */
                :1,                     /* Reserved */
                bpdu:1,                 /* BPDU packet */
                mirror:1,               /* Mirrored copy */
                msec_replay_error:1,    /* MACSEC replay error */
                msec_no_error:1,        /* MACSEC no error */
                :19;                    /* Reserved */

                                        /* T22.10 */
        uint32  :8,                     /* Reserved */
                timestamp_hi:24;        /* Timestamp (high) */

                                        /* T22.11 */
        uint32  timestamp_lo:8,         /* Timestamp (lo) */
                :11,                    /* Reserved */
                outer_tag_hi:13;        /* OTAG (high) */

                                        /* T22.12 */
        uint32  outer_tag_lo:3,         /* OTAG (low) */
                otag_action:2,          /* Ingress Outer tag action */
                sci_valid:1,            /* SCI index valid */
                sci_index:10,           /* SCI index */
                dei:1,                  /* DEI */
                mpls_exp:3,             /* MPLS EXP */
                vlan_chk_disable:1,     /* VLAN checks were disabled */
                unmodified:1,           /* Packet was unmodified */
                msec_decap_status:2,    /* MACSEC decap status */
                :3,                     /* Reserved */
                srcport:5;              /* Source port number */

                                        /* T22.13 */
        uint32  time_req:1,             /* Timestamping required by destport */
                l4_type:2,              /* L4 protocol */
                l4_ptr:8,               /* Pointer to L4 header */
                l3_type:3,              /* L3 protocol */
                l3_ptr:8,               /* Pointer to L3 header */
                num_labels:2,           /* Number of MPLS labels */
                mpls_ptr:8;             /* Pointer to first MPLS label */

                                        /* T22.14 */
        uint32  num_tags:2,             /* Number of tags */
                dot1q_ptr:7,            /* Pointer to VLAN tag(s) */
                frame_type:3,           /* Frame type */
                etype_ptr:7,            /* Ethertype pointer */
                destport:5,             /* Destination port */
                cell_error:1,           /* Cell error */
                change_ecn:1,           /* ECN change */
                ecn:2,                  /* ECN */
                cng:2,                  /* Congestion */
                src_hg:1,               /* Source is Higig */
                :1;                     /* Reserved */

                                        /* T22.15: DMA Status 0 */
        uint32  done:1,                 /* Descriptor Done */
                dc:12,                  /* Don't Care */
                error:1,                /* Cell Error (RX) */
                start:1,                /* Start bit (RX) */
                end:1,                  /* End bit (RX) */
                count:16;               /* Transferred byte count */
#endif
} dcb22_t;
#endif  /* BCM_SHADOW_SUPPORT */




#if  defined(BCM_PETRA_SUPPORT)
typedef struct {
        uint32  addr;                   /* T9.0: physical address */
#ifdef  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
                c_chain:1,              /* Chaining */
                c_sg:1,                 /* Scatter Gather */
                c_reload:1,             /* Reload */
                c_hg:1,                 /* Higig (TX) */
                c_stat:1,               /* update stats (TX) */
                c_pause:1,              /* Pause packet (TX) */
                c_purge:1,              /* Purge packet (TX) */
                :9;                     /* Don't care */
#else
        uint32  :9,                     /* Don't care */
                c_purge:1,              /* Purge packet (TX) */
                c_pause:1,
                c_stat:1,
                c_hg:1,
                c_reload:1,
                c_sg:1,
                c_chain:1,
                c_count:16;
#endif  /* LE_HOST */
        uint32  mh0;                    /* T9.2: Module Header word 0 */
        uint32  mh1;                    /* T9.3: Module Header word 1 */
        uint32  mh2;                    /* T9.4: Module Header word 2 */
        uint32  mh3;                    /* T11.5: Module Header word 3 */
#ifdef  LE_HOST
        uint32  mtp_index_hi:5,         /* Mirror-to-Port Index */
                l3uc:1,                 /* L3 UC */
                l3only:1,               /* L3 only */
                l3ipmc:1,               /* L3 IPMC */
                l3_intf:12,             /* L3 Intf number */
                imirror:1,              /* Ingress Mirroring */
                emirror:1,              /* Egress Mirroring */
                cos:3,                  /* Packet Priority */
                cpu_cos:3,              /* CPU COS */
                chg_tos:1,              /* DSCP Changed */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                bpdu:1,                 /* BPDU Packet */
                add_vid:1;              /* VLAN ID Added */
        uint32  reason:25,              /* CPU opcode */
                regen_crc:1,            /* Regenerate CRC */
                switch_pkt:1,           /* Switched packet */
                src_hg:1,               /* Source is Higig */
                purge_cell:1,           /* Packet is marked Purged */
                pkt_aged:1,             /* Pkt is Aged */
                mtp_index_lo:2;         /* Mirror-to-Port Index */
        uint32  dscp_hi:2,              /* New DSCP */
                srcport:5,              /* Source port */
                nh_index:13,            /* Next hop index */
                match_rule:10,          /* Matched Rule */
                match_rule_valid:1,     /* Matched Rule valid */
                decap_iptunnel:1;       /* Decap IP Tunneling Packet */
        uint32  :11,
                ingress_untagged:1,     /* Pkt came in untagged */
                outer_vid:12,           /* VID */
                outer_cfi:1,            /* CFI */
                outer_pri:3,            /* Priority */
                dscp_lo:4;              /* New DSCP */
#else
        uint32  add_vid:1,              /* VLAN ID Added */
                bpdu:1,                 /* BPDU Packet */
                cell_error:1,           /* Cell CRC Checksum Error Detected */
                chg_tos:1,              /* DSCP Changed */
                cpu_cos:3,              /* CPU COS */
                cos:3,                  /* Packet Priority */
                emirror:1,              /* Egress Mirroring */
                imirror:1,              /* Ingress Mirroring */
                l3_intf:12,             /* L3 Intf number */
                l3ipmc:1,               /* L3 IPMC */
                l3only:1,               /* L3 only */
                l3uc:1,                 /* L3 UC */
                mtp_index_hi:5;         /* Mirror-to-Port Index */
        uint32  mtp_index_lo:2,         /* Mirror-to-Port Index */
                pkt_aged:1,             /* Pkt is Aged */
                purge_cell:1,           /* Packet is marked Purged */
                src_hg:1,               /* Source is Higig */
                switch_pkt:1,           /* Switched packet */
                regen_crc:1,            /* Regenerate CRC */
                reason:25;              /* CPU opcode */
        uint32  decap_iptunnel:1,       /* Decap IP Tunneling Packet */
                match_rule_valid:1,     /* Matched Rule valid */
                match_rule:10,          /* Matched Rule */
                nh_index:13,            /* Next hop index */
                srcport:5,              /* Source port */
                dscp_hi:2;              /* New DSCP */
        uint32  dscp_lo:4,              /* New DSCP */
                outer_pri:3,            /* Priority (D)*/
                outer_cfi:1,            /* CFI (D)*/
                outer_vid:12,           /* VID (D)*/
                ingress_untagged:1,     /* Pkt came in untagged (D)*/
                :11;
#endif
        uint32  s_4;                    /* T9.9: RX Status 4 */
        uint32  s_41;                    /* T9.9: RX Status 4 */
        uint32  s_42;                    /* T9.9: RX Status 4 */
        uint32  s_43;                    /* T9.9: RX Status 4 */
        uint32  s_44;                    /* T9.9: RX Status 4 */
                                        /* T9.10: DMA Status 0 */
#ifdef  LE_HOST
        uint32  count:16,               /* Transferred byte count */
                end:1,                  /* End bit (RX) */
                start:1,                /* Start bit (RX) */
                error:1,                /* Cell Error (RX) */
                dc:12,                  /* Don't Care */
                done:1;                 /* Descriptor Done */
#else
        uint32  done:1,
                dc:12,
                error:1,
                start:1,
                end:1,
                count:16;
#endif
} dcb28_t;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT) || \
    defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_GREYHOUND_SUPPORT)
/*
 * DMA Control Block - Type 23
 * Used on 56640 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type23.h>

#endif  /* BCM_TRIUMPH3_SUPPORT || BCM_KATANA_SUPPORT || \
           BCM_TRIDENT2_SUPPORT || BCM_GREYHOUND_SUPPORT */

#if defined(BCM_KATANA_SUPPORT)
/*
 * DMA Control Block - Type 24
 * Used on 56440 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type24.h>

#endif  /* BCM_KATANA_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT)
/*
 * DMA Control Block - Type 26
 * Used on 56850 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type26.h>

#endif  /* BCM_TRIDENT2_SUPPORT */

#if defined(BCM_KATANA2_SUPPORT) 
/*
 * DMA Control Block - Type 29
 * Used on 56450 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type29.h>

#endif  /* BCM_KATANA2_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT) 
/*
 * DMA Control Block - Type 31
 * Used on 53400 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type31.h>

#endif  /* BCM_GREYHOUND_SUPPORT */

#if defined(BCM_TOMAHAWK_SUPPORT)
/*
 * DMA Control Block - Type 32
 * Used on 5696X devices
 * 16 words
 */
#include <soc/shared/dcbformats/type32.h>

#endif  /* BCM_TOMAHAWK_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
 * DMA Control Block - Type 33
 * Used on 56860 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type33.h>

#endif  /* BCM_TRIDENT2PLUS_SUPPORT */

#if defined(BCM_HURRICANE3_SUPPORT)
/*
 * DMA Control Block - Type 34
 * Used on 56160 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type34.h>

#endif  /* BCM_HURRICANE3_SUPPORT */

#if defined(BCM_APACHE_SUPPORT)
/*
 * DMA Control Block - Type 35
 * Used on 56560 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type35.h>

#endif  /* BCM_APACHE_SUPPORT */

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * DMA Control Block - Type 36
 * Used on 56870 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type36.h>

#endif  /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_GREYHOUND2_SUPPORT)
/*
 * DMA Control Block - Type 37
 * Used on XXXXX devices
 * 16 words
 */
#include <soc/shared/dcbformats/type37.h>

#endif  /* BCM_GREYHOUND2_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
/*
 * DMA Control Block - Type 38
 * Used on 56980 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type38.h>

#endif  /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_FIRELIGHT_SUPPORT)
/*
 * DMA Control Block - Type 40
 * Used on 56070 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type40.h>
#endif  /* BCM_FIRELIGHT_SUPPORT */
#if defined(BCM_DNX_SUPPORT)
/*
 * DMA Control Block - Type 39
 * Used on 88690 devices
 * 16 words
 */
#include <soc/shared/dcbformats/type39.h>

#endif  /* BCM_DNX_SUPPORT */

#endif  /* _SOC_DCBFORMATS_H */
