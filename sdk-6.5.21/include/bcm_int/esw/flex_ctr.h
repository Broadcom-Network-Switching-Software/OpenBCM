/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flex_ctr.h
 * Purpose:     Contains all internal function and structure required for
 *              flex counter.
 */

#ifndef __BCM_FLEX_CTR_H__
#define __BCM_FLEX_CTR_H__

#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/l2x.h>
#include <bcm/l2.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/esw/mim.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident.h>
#include <soc/firebolt6.h>
#include <bcm_int/esw/stat.h>

/*
 ADDITIONAL INFO
 Packet attributes are of three types
 1) Packet Attributes                i.e. uncompressed mode
 2) UDF attributes                   i.e. udf mode
 3) Compressed Packet Attributes     i.e. compressed mode
*/

typedef enum bcm_stat_flex_packet_attr_type_e {
    bcmStatFlexPacketAttrTypeUncompressed=0,
    bcmStatFlexPacketAttrTypeCompressed=1,
    bcmStatFlexPacketAttrTypeUdf=2
}bcm_stat_flex_packet_attr_type_t;

/*
 ========================================================================
 Total 12 packets attributes are supported in ingress packet flow direction
 This includes

 1) cng             (2b: Pre FP packet color)
 2) IFP_CNG         (2b: Post FP packet color)
 3) INT_PRI         (4b: Packet internal priority)

 ######################################################################
 ****** In compressed mode, attributes(cng,IFP_CNG,INT_PRI) are
 ****** grouped into PRI_CNG_MAP map table.
 ######################################################################

 4) VLAN_FORMAT     (2b:Incoming PacketInner&Outer Vlan tag present indicators)
 5) OUTER_DOT1P     (3b: Incoming packet outer vlan tag priority.)
 6) INNER_DOT1P     (3b: Incoming packet inner vlan tag priority.)
 ######################################################################
 ****** In compressed mode, attributes(VLAN_FORMAT,OUTER_DOT1P,INNER_DOT1P) are
 ****** grouped into PKT_PRI_MAP map table
 ######################################################################

 7) INGRESS_PORT    (6b: Packet local ingress port.)
 ######################################################################
 ****** In compressed mode, attributes(INGRESS_PORT) are
 ****** grouped into PORT_MAP map table
 ######################################################################

 8) TOS             (8b: Incoming IP packet TOS byte.)
 ######################################################################
 ****** In compressed mode, attributes(TOS) are
 ****** grouped into TOS_MAP map table
 ######################################################################

 9) PACKET_RESOLUTION(6b: IFP packet resolution vector.)
 10)SVP_TYPE         (1b:SVP(SourceVirtualPortNumber)is network port indication)
 11)DROP             (1b: Drop indication.)
 ######################################################################
 ****** In compressed mode, attributes(PACKET_RESOLUTION,SVP_TYPE,DROP) are
 ****** grouped into PKT_RES_MAP map table
 ######################################################################

 12)IP Packet         (1b: Packet is IP.)
 Total 12 Packet attributes with 39 bits
 ========================================================================
*/

typedef struct bcm_stat_flex_ing_pkt_attr_bits_s {
    /* 2 bits for Flow control packet type */
    uint8 fc_type;
    uint8 fc_type_pos;
    uint8 fc_type_mask;

    /* Addition of TCP Flags and Internal Congestion selectors
     * Total additional Bits : 8 + 2
     *                       : 10 bits
     */
    uint8 int_cn;
    uint8 int_cn_pos;
    uint8 int_cn_mask;

    uint8 tcp_flags;
    uint8 tcp_flags_pos;
    uint8 tcp_flags_mask;

    /*
    Below are default bits for uncompressed format.
    Need to be set for compressed format
    Total Bits :(2+2+4)+(2+3+3)+(6)+(8)+(6+1+1) + (1)
               : 8     + 8     + 6 + 8 + 8      + 1
               : 39 bits
    */
    /*
     * APACHE has Extended Flex Counter Packet Attributes
     * Total Bits : (4+4+4+4+4+4) + Existing as above
     *            : 24 + 39
     *            : 63 bits
     */
    uint8   cng_3;                /* 2 for uncompressed frmt   */
    uint8   cng_3_pos;
    uint8   cng_3_mask;

    uint8   phb_3;                /* 4 for uncompressed frmt   */
    uint8   phb_3_pos;
    uint8   phb_3_mask;

    uint8   cng_2;                /* 2 for uncompressed frmt   */
    uint8   cng_2_pos;
    uint8   cng_2_mask;

    uint8   phb_2;                /* 4 for uncompressed frmt   */
    uint8   phb_2_pos;
    uint8   phb_2_mask;

    uint8   cng_1;                /* 2 for uncompressed frmt   */
    uint8   cng_1_pos;
    uint8   cng_1_mask;

    uint8   phb_1;                /* 4 for uncompressed frmt   */
    uint8   phb_1_pos;
    uint8   phb_1_mask;
    /* PRI_CNF Group for compressed format */
    uint8   cng;                  /* 2 for uncompressed frmt   */
    uint8   cng_pos;
    uint8   cng_mask;

    uint8   ifp_cng;              /* 2 for uncompressed format */
    uint8   ifp_cng_pos;
    uint8   ifp_cng_mask;

    uint8   int_pri;              /* 4 for uncompressed format */
    uint8   int_pri_pos;
    uint8   int_pri_mask;

    /* PKT_PRI vlan Group for compressed format */
    uint8   vlan_format;          /* 2 for uncompressed format */
    uint8   vlan_format_pos;
    uint8   vlan_format_mask;
    uint8   outer_dot1p;          /* 3 for uncompressed format */
    uint8   outer_dot1p_pos;
    uint8   outer_dot1p_mask;
    uint8   inner_dot1p;          /* 3 for uncompressed format */
    uint8   inner_dot1p_pos;
    uint8   inner_dot1p_mask;

    /* Port Group for compressed format */
    uint8   ing_port;             /* 6 for uncompressed format */
    uint8   ing_port_pos;
    uint8   ing_port_mask;

    /* TOS Group for compressed format  */
    uint8   tos_dscp;                  /* 8 for uncompressed format */
    uint8   tos_dscp_pos;
    uint8   tos_dscp_mask;
    uint8   tos_ecn;                  /* 8 for uncompressed format */
    uint8   tos_ecn_pos;
    uint8   tos_ecn_mask;
    /* PKT_RESOLTION Group for compressed format */
    uint8   pkt_resolution;       /* 6 for uncompressed format */
    uint8   pkt_resolution_pos;
    uint8   pkt_resolution_mask;

    uint8   svp_type;             /* 1 for uncompressed format */
    uint8   svp_type_pos;
    uint8   svp_type_mask;

    uint8   drop;                 /* 1 for uncompressed format */
    uint8   drop_pos;
    uint8   drop_mask;

    uint8   ip_pkt;               /* 1 for uncompressed format */
    uint8    ip_pkt_pos;
    uint8    ip_pkt_mask;
}bcm_stat_flex_ing_pkt_attr_bits_t;


/*
 ========================================================================
 Total 12 packets attributes are supported in egress packet flow direction
 This includes

 1) cng            (2b: Post FP packet color)
 2) INT_PRI        (4b: Packet internal priority)
 ######################################################################
 ****** In compressed mode, attributes(cng,INT_PRI) are
 ****** grouped into PRI_CNG_MAP map table.
 ######################################################################

 3) VLAN_FORMAT    (2b: OutgoingPacketInner&Outer Vlan tag present indicators.)
 4) OUTER_DOT1P    (3b: Incoming packet outer vlan tag priority.)
 5) INNER_DOT1P    (3b: Incoming packet inner vlan tag priority.)
 ######################################################################
 ****** In compressed mode, attributes(VLAN_FORMAT,OUTER_DOT1P,INNER_DOT1P) are
 ****** grouped into PKT_PRI_MAP map table
 ######################################################################

 6) EGRESS_PORT     (6b: Packet local egress port.)
 ######################################################################
 ****** In compressed mode, attributes(EGRESS_PORT) are
 ****** grouped into PORT_MAP map table
 ######################################################################

 7) TOS             (8b: Outgoing IP packet TOS byte.)
 ######################################################################
 ****** In compressed mode, attributes(TOS) are
 ****** grouped into TOS_MAP map table
 ######################################################################

 8) PACKET_RESOLUTION(1b: IFP packet resolution vector.)
 9)SVP_TYPE          (1b: SVP(SourceVirtualPortNumber) is NetworkPortindication)
 10)DVP_TYPE         (1b: DVP(DestinationVirtualPortNumber) is
                          NetworkPortIndication.)
 11)DROP             (1b: Drop indication.)
 ######################################################################
 ****** In compressed mode, attributes(PACKET_RESOLUTION,SVP_TYPE,DVP,DROP) are
 ****** grouped into PKT_RES_MAP map table
 12)IP Packet         (1b: Packet is IP.)
 Total 12 Packet attributes with 33 bits
 ========================================================================
*/
typedef struct bcm_stat_flex_egr_pkt_attr_bits_s {
    /* Addition of Internal Congestion selectors
     * Total additional Bits : 2 bits
     */
    uint8 int_cn;
    uint8 int_cn_pos;
    uint8 int_cn_mask;

    /*
    Below are default bits for uncompressed
    format. Need to be set for compressed format
    Total Bits :(2+4)+(2+3+3)+(6)+(8)+(1+1+1+1) + (1)
               : 6     + 8   + 6 + 8 + 4      + 1
               : 33 bits
    */
    uint8   mmu_cos;
    uint8   mmu_cos_pos;
    uint8   mmu_cos_mask;

    uint8   uc_queueing;
    uint8   uc_queueing_pos;
    uint8   uc_queueing_mask;

    uint8   congestion_marked;
    uint8   congestion_marked_pos;
    uint8   congestion_marked_mask;

    /*
     * APACHE has Extended Flex Counter Packet Attributes
     * Total Bits : (4+4+4+4+4+4) + Existing as above
     *            : 24 + 39
     *            : 63 bits
     */
    uint8   cng_3;                /* 2 for uncompressed frmt   */
    uint8   cng_3_pos;
    uint8   cng_3_mask;

    uint8   phb_3;                /* 4 for uncompressed frmt   */
    uint8   phb_3_pos;
    uint8   phb_3_mask;

    uint8   cng_2;                /* 2 for uncompressed frmt   */
    uint8   cng_2_pos;
    uint8   cng_2_mask;

    uint8   phb_2;                /* 4 for uncompressed frmt   */
    uint8   phb_2_pos;
    uint8   phb_2_mask;

    uint8   cng_1;                /* 2 for uncompressed frmt   */
    uint8   cng_1_pos;
    uint8   cng_1_mask;

    uint8   phb_1;                /* 4 for uncompressed frmt   */
    uint8   phb_1_pos;
    uint8   phb_1_mask;
    /* PRI_CNF Group for compressed format */
    uint8   cng;                  /* 2 for uncompressed frmt   */
    uint8   cng_pos;
    uint8   cng_mask;

    uint8   int_pri;              /* 4 for uncompressed format */
    uint8   int_pri_pos;
    uint8   int_pri_mask;

    /* PKT_PRI vlan Group for compressed format */
    uint8   vlan_format;         /* 2 for uncompressed format  */
    uint8   vlan_format_pos;
    uint8   vlan_format_mask;

    uint8   outer_dot1p;         /* 3 for uncompressed format  */
    uint8   outer_dot1p_pos;
    uint8   outer_dot1p_mask;

    uint8   inner_dot1p;         /* 3 for uncompressed format   */
    uint8   inner_dot1p_pos;
    uint8   inner_dot1p_mask;

    /* Port Group for compressed format */
    uint8   egr_port;            /* 6 for uncompressed format  */
    uint8   egr_port_pos;
    uint8   egr_port_mask;

    /* TOS Group for compressed format */
    uint8   tos_dscp;                 /* 8 for uncompressed format  */
    uint8   tos_dscp_pos;
    uint8   tos_dscp_mask;
    uint8   tos_ecn;                 /* 8 for uncompressed format  */
    uint8   tos_ecn_pos;
    uint8   tos_ecn_mask;

    /* PKT_RESOLTION Group for compressed format               */
    uint8   pkt_resolution;     /* 1 for uncompressed format   */
    uint8   pkt_resolution_pos;
    uint8   pkt_resolution_mask;

    uint8   svp_type;           /* 1 for uncompressed format   */
    uint8   svp_type_pos;
    uint8   svp_type_mask;

    uint8   dvp_type;           /* 1 for uncompressed format   */
    uint8   dvp_type_pos;
    uint8   dvp_type_mask;

    uint8   drop;               /* 1 for uncompressed format   */
    uint8   drop_pos;
    uint8   drop_mask;

    uint8   ip_pkt;             /* 1 for uncompressed format   */
    uint8   ip_pkt_pos;
    uint8   ip_pkt_mask;
}bcm_stat_flex_egr_pkt_attr_bits_t;

/*
 There are two UDF (UserDefinedFields) chunks each contains one 16-bit UDF bit.
 Each UDF chunk has a valid bit associated with it. UDF attributes are 35-bits
 and contains following fields
 1) UDF0 (16-bits)
 2) UDF1 (16-bits)
 3) UDF_VALID0 (1-bit)
 4) UDF_VALID1 (1-bit)
 5) Drop (1-bit)
 For a packet, one or both of the udf valid bits might be invalid..

 UDF configuration happens separately and for configuring flex-counter we need
 below two udf(16b) masks only
*/


typedef struct bcm_stat_flex_udf_pkt_attr_bits_s {
    uint16  udf0;
    uint16  udf1;
}bcm_stat_flex_udf_pkt_attr_bits_t;

/* For UncompressedIngressMode,PacketAttributes bits are fixed i.e.cng 2 bits */
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_ATTR_BITS            0x1
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_IFP_CNG_ATTR_BITS        0x2
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INT_PRI_ATTR_BITS        0x4
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_VLAN_FORMAT_ATTR_BITS    0x8
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_OUTER_DOT1P_ATTR_BITS    0x10
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INNER_DOT1P_ATTR_BITS    0x20
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INGRESS_PORT_ATTR_BITS   0x40
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_TOS_DSCP_ATTR_BITS       0x80
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_TOS_ECN_ATTR_BITS        0x100
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PKT_RESOLUTION_ATTR_BITS 0x200
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_SVP_TYPE_ATTR_BITS       0x400
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_DROP_ATTR_BITS           0x800
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_IP_PKT_ATTR_BITS         0x1000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_3_ATTR_BITS          0x2000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PHB_3_ATTR_BITS          0x4000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_2_ATTR_BITS          0x8000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PHB_2_ATTR_BITS          0x10000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_CNG_1_ATTR_BITS          0x20000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_PHB_1_ATTR_BITS          0x40000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_TCP_FLAG_ATTR_BITS       0x80000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_INT_CN_ATTR_BITS         0x100000
#define BCM_STAT_FLEX_ING_UNCOMPRESSED_USE_FC_TYPE_ATTR_BITS        0x200000

/* For UncompressedEgressMode,PacketAttributes bits are fixed i.e.cng 2 bits */

#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_ATTR_BITS            0x1
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_INT_PRI_ATTR_BITS        0x2
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_VLAN_FORMAT_ATTR_BITS    0x4
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_OUTER_DOT1P_ATTR_BITS    0x8
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_INNER_DOT1P_ATTR_BITS    0x10
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_EGRESS_PORT_ATTR_BITS    0x20
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_TOS_DSCP_ATTR_BITS       0x40
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_TOS_ECN_ATTR_BITS        0x80
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PKT_RESOLUTION_ATTR_BITS 0x100
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_SVP_TYPE_ATTR_BITS       0x200
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_DVP_TYPE_ATTR_BITS       0x400
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_DROP_ATTR_BITS           0x800
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_IP_PKT_ATTR_BITS         0x1000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_3_ATTR_BITS          0x2000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PHB_3_ATTR_BITS          0x4000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_2_ATTR_BITS          0x8000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PHB_2_ATTR_BITS          0x10000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CNG_1_ATTR_BITS          0x20000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_PHB_1_ATTR_BITS          0x40000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_MMU_COS_ATTR_BITS        0x80000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_UC_QUEUEING_ATTR_BITS    0x100000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_CONGESTION_MARKED_ATTR_BITS     0x200000
#define BCM_STAT_FLEX_EGR_UNCOMPRESSED_USE_INT_CN_ATTR_BITS                0x400000

/* Packet attributes are mapped down to an 8 bit key before being used
 * to index the counter offset table
 */
#define BCM_STAT_FLEX_MAX_PKT_ATTR_SEL_KEY_SIZE                     8


typedef struct bcm_stat_flex_offset_table_entry_s {
    uint8    offset;
    uint8    count_enable;
}bcm_stat_flex_offset_table_entry_t;

typedef struct bcm_stat_flex_ing_uncmprsd_attr_selectors_s {
    uint32                             uncmprsd_attr_bits_selector;
    uint16                             total_counters;
    bcm_stat_flex_offset_table_entry_t offset_table_map[256];
}bcm_stat_flex_ing_uncmprsd_attr_selectors_t;

typedef struct bcm_stat_flex_egr_uncmprsd_attr_selectors_s {
    uint32                             uncmprsd_attr_bits_selector;
    uint16                             total_counters;
    bcm_stat_flex_offset_table_entry_t offset_table_map[256];
}bcm_stat_flex_egr_uncmprsd_attr_selectors_t;


/*
   ###################################################
   Ingress side compressed mode maps
   ###################################################

   PRI_CNG_MAP[2^(cng:2b + ifp_cng:2b +int_pri:4b=8)=256]  *  8
   PKT_PRI_MAP[2^(VLAN_FORMAT:2b + OUTER_DOT1P:3b + INNER_DOT1P:3b=8)=256] * 8
   PORT_MAP[2^(INGRESS_PORT:6b=6)=64] * 8
   TOS_MAP[2^(TOS:8b=8)=256] * 8
   PKT_RES_MAP[2^(PACKET_RESOLUTION:6b + SVP_TYPE:1b + DROP:1b=8)=256]  * 8

*/
typedef uint8 bcm_stat_flex_ing_cmprsd_pri_cnf_attr_map_t[256]; /*256 * 8 */
typedef uint8 bcm_stat_flex_ing_cmprsd_pkt_pri_attr_map_t[256]; /*256 * 8 */
typedef uint8 bcm_stat_flex_ing_cmprsd_port_attr_map_t[512];    /*64  * 8 */
                                                                /*TD2:128*8 */
                                                                /*KT2:170*8 */
typedef uint8 bcm_stat_flex_ing_cmprsd_tos_attr_map_t[256];     /*256 * 8 */
                                                         /* KT:256 KT2:1024 */
typedef uint8 bcm_stat_flex_ing_cmprsd_pkt_res_attr_map_t[1024]; /*256 * 8 */

typedef struct bcm_stat_flex_ing_cmprsd_attr_selectors_s {
    uint8                                           cng_3;
    uint8                                           phb_3;
    uint8                                           cng_2;
    uint8                                           phb_2;
    uint8                                           cng_1;
    uint8                                           phb_1;
    bcm_stat_flex_ing_pkt_attr_bits_t               pkt_attr_bits;
    bcm_stat_flex_ing_cmprsd_pri_cnf_attr_map_t     pri_cnf_attr_map;
    bcm_stat_flex_ing_cmprsd_pkt_pri_attr_map_t     pkt_pri_attr_map;
    bcm_stat_flex_ing_cmprsd_port_attr_map_t        port_attr_map;
    bcm_stat_flex_ing_cmprsd_tos_attr_map_t         tos_attr_map;
    bcm_stat_flex_ing_cmprsd_pkt_res_attr_map_t     pkt_res_attr_map;
    uint8                                           total_counters;
    bcm_stat_flex_offset_table_entry_t              offset_table_map[256];
}bcm_stat_flex_ing_cmprsd_attr_selectors_t;

/*
   ###################################################
   Egress side compressed mode maps
   ###################################################

   PRI_CNG_MAP[2^(cng:2b + int_pri:4b=6)=64]  *  8
   PKT_PRI_MAP[2^(VLAN_FORMAT:2b + OUTER_DOT1P:3b + INNER_DOT1P:3b=8)=256] * 8
   PORT_MAP[2^(EGRESS_PORT:6b=6)=64] * 8
   TOS_MAP[2^(TOS:8b=8)=256] * 8
   PKT_RES_MAP[2^(PACKET_RESOLUTION:1b +
                  SVP_TYPE:1b +
                  DVP_TYPE:1b +
                  DROP:1b=4)=16]  * 8
*/
typedef uint8 bcm_stat_flex_egr_cmprsd_pri_cnf_attr_map_t[64];
typedef uint8 bcm_stat_flex_egr_cmprsd_pkt_pri_attr_map_t[256];
typedef uint8 bcm_stat_flex_egr_cmprsd_port_attr_map_t[170]; /* KT:64,KT2:169 */
typedef uint8 bcm_stat_flex_egr_cmprsd_tos_attr_map_t[256];
typedef uint8 bcm_stat_flex_egr_cmprsd_pkt_res_attr_map_t[256];
typedef uint8 bcm_stat_flex_egr_cmprsd_cos_uc_attr_map_t[64];

typedef struct bcm_stat_flex_egr_cmprsd_attr_selectors_s {
    uint8                                           cng_3;
    uint8                                           phb_3;
    uint8                                           cng_2;
    uint8                                           phb_2;
    uint8                                           cng_1;
    uint8                                           phb_1;
    bcm_stat_flex_egr_pkt_attr_bits_t           pkt_attr_bits;
    bcm_stat_flex_egr_cmprsd_pri_cnf_attr_map_t pri_cnf_attr_map;
    bcm_stat_flex_egr_cmprsd_pkt_pri_attr_map_t pkt_pri_attr_map;
    bcm_stat_flex_egr_cmprsd_port_attr_map_t    port_attr_map;
    bcm_stat_flex_egr_cmprsd_tos_attr_map_t     tos_attr_map;
    bcm_stat_flex_egr_cmprsd_pkt_res_attr_map_t pkt_res_attr_map;
    bcm_stat_flex_egr_cmprsd_cos_uc_attr_map_t  cos_uc_attr_map;
    uint8                                       total_counters;
    bcm_stat_flex_offset_table_entry_t          offset_table_map[256];
}bcm_stat_flex_egr_cmprsd_attr_selectors_t;

typedef struct bcm_stat_flex_udf_pkt_attr_selectors_s {
    bcm_stat_flex_udf_pkt_attr_bits_t    udf_pkt_attr_bits;
    uint32 drop_enable;
    uint8  total_counters;
}bcm_stat_flex_udf_pkt_attr_selectors_t;


typedef    struct bcm_stat_flex_ing_attr_s {
    bcm_stat_flex_packet_attr_type_t                packet_attr_type;
    /* union of all possible ingress packet attributes selectors
    i.e. uncompressed,compressed and udf */
    bcm_stat_flex_ing_uncmprsd_attr_selectors_t     uncmprsd_attr_selectors;
    bcm_stat_flex_ing_cmprsd_attr_selectors_t       cmprsd_attr_selectors;
    bcm_stat_flex_udf_pkt_attr_selectors_t          udf_pkt_attr_selectors;
}bcm_stat_flex_ing_attr_t;

typedef    struct bcm_stat_flex_egr_attr_s {
    bcm_stat_flex_packet_attr_type_t                packet_attr_type;
    /* union of all possible egress packet attributes selectors i.e.
    uncompressed,compressed and udf */
    bcm_stat_flex_egr_uncmprsd_attr_selectors_t     uncmprsd_attr_selectors;
    bcm_stat_flex_egr_cmprsd_attr_selectors_t       cmprsd_attr_selectors;
    bcm_stat_flex_udf_pkt_attr_selectors_t          udf_pkt_attr_selectors;
}bcm_stat_flex_egr_attr_t;

typedef    struct bcm_stat_flex_attr_s {
    bcm_stat_flex_direction_t   direction;
    bcm_stat_flex_ing_attr_t    ing_attr;
    bcm_stat_flex_egr_attr_t    egr_attr;
}bcm_stat_flex_attr_t;

/* flex counter mode. */
/* MSB bit will indicate ingress or egress mode. */
typedef uint32  bcm_stat_flex_mode_t;


typedef struct bcm_stat_flex_counter_value_s {
    uint32    pkt_counter_value;
    uint64    byte_counter_value;
    uint64    pkt64_counter_value;
}bcm_stat_flex_counter_value_t;

typedef struct bcm_stat_flex_ctr_offset_info_s {
    uint8    all_counters_flag;
    uint32   offset_index; /* Valid only when all_counters_flag is false=0*/
}bcm_stat_flex_ctr_offset_info_t;


/*  FLEX COUNTER COMMON DECLARATION */

#define BCM_STAT_FLEX_COUNTER_MAX_DIRECTION         2
#define BCM_STAT_FLEX_COUNTER_MAX_MODE              4
#define BCM_STAT_FLEX_COUNTER_MAX_POOL              16
#define BCM_STAT_FLEX_COUNTER_MAX_INGRESS_TABLE     16
#define BCM_STAT_FLEX_COUNTER_MAX_TH_IFP_INGRESS_TABLE  20
#define BCM_STAT_FLEX_COUNTER_MAX_EGRESS_TABLE      8
#define BCM_STAT_FLEX_COUNTER_MAX_SCACHE_SIZE       32
#define BCM_STAT_FLEX_COUNTER_MAX_TOTAL_BITS        8
#define BCM_STAT_FLEX_COUNTER_POOLS_PER_GROUP       4
#define BCM_STAT_FLEX_COUNTER_MAX_GROUPS     (BCM_STAT_FLEX_COUNTER_MAX_POOL/ \
                                              BCM_STAT_FLEX_COUNTER_POOLS_PER_GROUP)
#define BCM_STAT_TH_FLEX_COUNTER_MAX_POOL           20 
#define BCM_STAT_AP_FLEX_COUNTER_MAX_PKT_ATTR_SEL   32


#define BCM_STAT_DEF_FLEX_COUNTER_MAX_GROUPS  1
#define BCM_STAT_TR3_FLEX_COUNTER_MAX_GROUPS  2
#define BCM_STAT_TD2_FLEX_COUNTER_MAX_GROUPS  2

#define BCM_STAT_AP_FLEX_COUNTER_MAX_GROUPS   8


/* Tomahawk has 4 additional pools */
#define BCM_STAT_TH_FLEX_COUNTER_MAX_GROUPS  (BCM_STAT_TH_FLEX_COUNTER_MAX_POOL/ \
                                              BCM_STAT_FLEX_COUNTER_POOLS_PER_GROUP)

#define BCM_STAT_GROUP_MODE_ID_OAM_SUFFIX 0x10000000 /* Suffix for Stat Group Mode With OAM */
#define BCM_STAT_GROUP_MODE_ID_OAM_MASK   0xF0000000 /* Mask for Stat Group Mode With OAM */



#define BCM_STAT_FLEX_COUNTER_MAX_ATTR_SEL_BITS 8
#define BCM_STAT_FLEX_MAX_COUNTERS_PER_MODE     256
#define BCM_FLEX_STAT_TH_RSVD_IFP_POOL_MIN      12
#define BCM_FLEX_STAT_TH_RSVD_IFP_POOL_MAX      19

#define BCM_FLEX_STAT_HX5_RSVD_IFP_POOL_MIN      8

#define BCM_STAT_TD3_FLEX_COUNTER_MAX_EGR_POOL   4

typedef struct bcm_stat_flex_table_mode_info_t {
    soc_mem_t            soc_mem_v;
    bcm_stat_flex_mode_t mode;
}bcm_stat_flex_table_mode_info_t;


/* POOL NUMBERS for INGRESS Tables */


#define FLEX_COUNTER_DEFAULT_PORT_TABLE_POOL_NUMBER                        0
#define FLEX_COUNTER_DEFAULT_VFP_POLICY_TABLE_POOL_NUMBER                  1
/* VFI and VLAN sharing same pool id */
#define FLEX_COUNTER_DEFAULT_VFI_TABLE_POOL_NUMBER                         2
#define FLEX_COUNTER_DEFAULT_VLAN_TABLE_POOL_NUMBER                        2
#define FLEX_COUNTER_DEFAULT_VXLAN_TABLE_POOL_NUMBER                       2

/* VLAN_XLATE and MPLS_TUNNEL_LABEL sharing same pool id */
#define FLEX_COUNTER_DEFAULT_VLAN_XLATE_TABLE_POOL_NUMBER                  3
#define FLEX_COUNTER_DEFAULT_MPLS_ENTRY_TABLE_POOL_NUMBER                  3

/* MPLS_ENTRY_VC_LABEL and VRF can share same pool */
#define FLEX_COUNTER_DEFAULT_VRF_TABLE_POOL_NUMBER                         4
#define FLEX_COUNTER_DEFAULT_AGM_MONITOR_TABLE_POOL_NUMBER                 4

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#define FLEX_COUNTER_DEFAULT_VLAN_XLATE_SCND_LKUP_TABLE_POOL_NUMBER        4
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

/* Source VP and L3_iif sharing same pool id */
#define FLEX_COUNTER_DEFAULT_L3_IIF_TABLE_POOL_NUMBER                      5
#define FLEX_COUNTER_DEFAULT_SOURCE_VP_TABLE_POOL_NUMBER                   5
#define FLEX_COUNTER_DEFAULT_AGM_MONITOR_TABLE_SCND_LKUP_POOL_NUMBER       5

#if defined(BCM_APACHE_SUPPORT)
#define FLEX_COUNTER_DEFAULT_MPLS_ENTRY_SCND_LKUP_TABLE_POOL_NUMBER        5
#endif /* BCM_APACHE_SUPPORT */


#define FLEX_COUNTER_DEFAULT_L3_TUNNEL_TABLE_POOL_NUMBER                   6
#define FLEX_COUNTER_DEFAULT_FCOE_TABLE_POOL_NUMBER                        6

/* L3_ENTRY_2/4 + EXT_IPV4_UCAST_WIDE/EXT_IPV6_UCAST_WIDE share same pool id */
#define FLEX_COUNTER_DEFAULT_L3_ENTRY_TABLE_POOL_NUMBER                    7
#define FLEX_COUNTER_DEFAULT_L3_ROUTE_TABLE_POOL_NUMBER                    7

#define FLEX_COUNTER_DEFAULT_EXT_FP_POLICY_TABLE_POOL_NUMBER               8
#define FLEX_COUNTER_DEFAULT_IFP_POLICY_TABLE_POOL_NUMBER                  8

#define FLEX_COUNTER_DEFAULT_EM_POLICY_TABLE_POOL_NUMBER                   9

#ifdef BCM_HELIX5_SUPPORT
#define FLEX_COUNTER_HX5_DEFAULT_EM_POLICY_TABLE_POOL_NUMBER               7
#define FLEX_COUNTER_HX5_EXT_FP_POLICY_TABLE_POOL_NUMBER                   7
#endif

/* Support for MetroLite*/
#if defined(BCM_METROLITE_SUPPORT)
#define FLEX_COUNTER_ML_DEFAULT_VRF_TABLE_POOL_NUMBER                      2
#define FLEX_COUNTER_ML_DEFAULT_AGM_MONITOR_TABLE_POOL_NUMBER              2

#define FLEX_COUNTER_ML_DEFAULT_L3_IIF_TABLE_POOL_NUMBER                   3
#define FLEX_COUNTER_ML_DEFAULT_SOURCE_VP_TABLE_POOL_NUMBER                3

#define FLEX_COUNTER_ML_DEFAULT_L3_TUNNEL_TABLE_POOL_NUMBER                2
#define FLEX_COUNTER_ML_DEFAULT_FCOE_TABLE_POOL_NUMBER                     2

/* L3_ENTRY_2/4 + EXT_IPV4_UCAST_WIDE/EXT_IPV6_UCAST_WIDE share same pool id */
#define FLEX_COUNTER_ML_DEFAULT_L3_ENTRY_TABLE_POOL_NUMBER                 3
#define FLEX_COUNTER_ML_DEFAULT_L3_ROUTE_TABLE_POOL_NUMBER                 3

#define FLEX_COUNTER_ML_DEFAULT_EXT_FP_POLICY_TABLE_POOL_NUMBER            2
#define FLEX_COUNTER_ML_DEFAULT_IFP_POLICY_TABLE_POOL_NUMBER               2

#define FLEX_COUNTER_ML_DEFAULT_EM_POLICY_TABLE_POOL_NUMBER                2
#endif /* BCM_METROLITE_SUPPORT */


/* POOL MASKS for INGRESS Tables */
#define FLEX_COUNTER_POOL_USED_BY_PORT_TABLE                               0x1
#define FLEX_COUNTER_POOL_USED_BY_VFP_POLICY_TABLE                         0x2
#define FLEX_COUNTER_POOL_USED_BY_VLAN_TABLE                               0x4
#define FLEX_COUNTER_POOL_USED_BY_VFI_TABLE                                0x8
#define FLEX_COUNTER_POOL_USED_BY_VLAN_XLATE_TABLE                         0x10
#define FLEX_COUNTER_POOL_USED_BY_MPLS_ENTRY_TABLE                         0x20
#define FLEX_COUNTER_POOL_USED_BY_VRF_TABLE                                0x40
#define FLEX_COUNTER_POOL_USED_BY_L3_IIF_TABLE                             0x80
#define FLEX_COUNTER_POOL_USED_BY_SOURCE_VP_TABLE                          0x100
#define FLEX_COUNTER_POOL_USED_BY_L3_TUNNEL_TABLE                          0x200
#define FLEX_COUNTER_POOL_USED_BY_L3_ENTRY_TABLE                           0x400
#define FLEX_COUNTER_POOL_USED_BY_EXT_FP_POLICY_TABLE                      0x800
#define FLEX_COUNTER_POOL_USED_BY_VXLAN_TABLE                              0x1000
#define FLEX_COUNTER_POOL_USED_BY_FCOE_TABLE                               0x2000
#define FLEX_COUNTER_POOL_USED_BY_VSAN_TABLE                               0x4000
#define FLEX_COUNTER_POOL_USED_BY_L3_ROUTE_TABLE                           0x8000
#define FLEX_COUNTER_POOL_USED_BY_IFP_POLICY_TABLE                         0x10000
#define FLEX_COUNTER_POOL_USED_BY_AGM_MONITOR_TABLE                        0x20000
#define FLEX_COUNTER_POOL_USED_BY_EM_POLICY_TABLE                          0x40000
#define FLEX_COUNTER_POOL_USED_BY_VLAN_XLATE_TABLE_FOR_SCND_LKUP           0x80000
#define FLEX_COUNTER_POOL_USED_BY_MPLS_ENTRY_TABLE_FOR_SCND_LKUP           0x100000
#define FLEX_COUNTER_POOL_USED_BY_AGM_MONITOR_TABLE_FOR_SCND_LKUP          0x200000

/* POOL NUMBERS for EGRESS Tables */
#define FLEX_COUNTER_DEFAULT_EGR_L3_NEXT_HOP_TABLE_POOL_NUMBER             0
#define FLEX_COUNTER_DEFAULT_EGR_VFI_TABLE_POOL_NUMBER                     1
#define FLEX_COUNTER_DEFAULT_EGR_PORT_TABLE_POOL_NUMBER                    2
#define FLEX_COUNTER_DEFAULT_EGR_VLAN_TABLE_POOL_NUMBER                    3
#define FLEX_COUNTER_DEFAULT_EGR_VLAN_XLATE_TABLE_POOL_NUMBER              4
#define FLEX_COUNTER_DEFAULT_EGR_DVP_ATTRIBUTE_1_TABLE_POOL_NUMBER         5
#define FLEX_COUNTER_DEFAULT_L3_NAT_TABLE_POOL_NUMBER                      7
#if defined(BCM_TOMAHAWK3_SUPPORT)
#define FLEX_COUNTER_DEFAULT_LATENCY_MONITOR_POOL_NUMBER                   3
#endif

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_METROLITE_SUPPORT)
#define FLEX_COUNTER_TD2_DEFAULT_EGR_VLAN_XLATE_TABLE_POOL_NUMBER          0
#define FLEX_COUNTER_TD2_DEFAULT_EGR_DVP_ATTRIBUTE_1_TABLE_POOL_NUMBER     1
#define FLEX_COUNTER_TD2_DEFAULT_L3_NAT_TABLE_POOL_NUMBER                  2
#endif

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#define FLEX_COUNTER_DEFAULT_EGR_IP_TUNNEL_MPLS_POOL_NUMBER                2
#define FLEX_COUNTER_DEFAULT_EGR_IP_TUNNEL_MPLS_SCND_LBL_POOL_NUMBER       3
#define FLEX_COUNTER_DEFAULT_EGR_EFP_POLICY_TABLE_POOL_NUMBER              3
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

 #if defined(BCM_APACHE_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
 #define FLEX_COUNTER_DEFAULT_EGR_VLAN_XLATE_SCND_LKUP_POOL_NUMBER         2
 #endif /* BCM_APACHE_SUPPORT */

/* POOL MASKS for EGRESS Tables */
#define FLEX_COUNTER_POOL_USED_BY_EGR_L3_NEXT_HOP_TABLE                    0x1
#define FLEX_COUNTER_POOL_USED_BY_EGR_VFI_TABLE                            0x2
#define FLEX_COUNTER_POOL_USED_BY_EGR_PORT_TABLE                           0x4
#define FLEX_COUNTER_POOL_USED_BY_EGR_VLAN_TABLE                           0x8
#define FLEX_COUNTER_POOL_USED_BY_EGR_VLAN_XLATE_TABLE                     0x10
#define FLEX_COUNTER_POOL_USED_BY_EGR_DVP_ATTRIBUTE_1_TABLE                0x20
#define FLEX_COUNTER_POOL_USED_BY_EGR_L3_NAT_TABLE                         0x40
#define FLEX_COUNTER_POOL_USED_BY_EGR_EFP_POLICY_TABLE                     0x80
#define FLEX_COUNTER_POOL_USED_BY_EGR_IP_TUNNEL_MPLS_TABLE                0x100
#define FLEX_COUNTER_POOL_USED_BY_EGR_VLAN_XLATE_TABLE_FOR_SCND_LKUP      0x200
#define FLEX_COUNTER_POOL_USED_BY_EGR_IP_TUNNEL_MPLS_SCND_LBL_TABLE       0x400
#define FLEX_COUNTER_POOL_USED_BY_LATENCY_MONITOR_TABLE                   0x800


#define TABLE_INDEPENDENT_POOL_MASK  0x80000000

/* ================================================================= */
/* Pkt Resolution bits for Triumph3 is 6 bits so max value is 64     */
/* Below values are just index and nothing to do with actual value   */
/* Actual chip specific values will be determined  in init function  */
/* ================================================================= */
#define _UNKNOWN_PKT       0       /* unknown packet */
#define _CONTROL_PKT       1       /* pkt(12, 13) == 0x8808 */
#define _BPDU_PKT          2       /* L2 USER ENTRY table BPDU bit */
#define _L2BC_PKT          3       /* L2 Broadcast pkt */
#define _L2UC_PKT          4       /* L2 Unicast pkt */
#define _L2DLF_PKT         5       /* L2 destination lookup failure */
#define _UNKNOWN_IPMC_PKT  6       /* Unknown IP Multicast pkt */
#define _KNOWN_IPMC_PKT    7       /* Known IP Multicast pkt */
#define _KNOWN_L2MC_PKT    8       /* Known L2 multicast pkt */
#define _UNKNOWN_L2MC_PKT  9       /* Unknown L2 multicast pkt */
#define _KNOWN_L3UC_PKT    10       /* Known L3 Unicast pkt */
#define _UNKNOWN_L3UC_PKT  11       /* Unknown L3 Unicast pkt */
#define _KNOWN_MPLS_PKT    12       /* Known MPLS pkt */
#define _KNOWN_MPLS_L3_PKT 13       /* Known L3 MPLS pkt */
#define _KNOWN_MPLS_L2_PKT 14       /* Known L2 MPLS pkt */
#define _UNKNOWN_MPLS_PKT  15       /* Unknown MPLS pkt */
#define _KNOWN_MIM_PKT     16       /* Known MIM pkt */
#define _UNKNOWN_MIM_PKT   17       /* Unknown MIM pkt */
#define _KNOWN_MPLS_MULTICAST_PKT    18       /* Known MPLS multicast pkt */

/* Additional Control Packets */
#define _OAM_PKT           19       /* OAM packet that needs to be 
                                       terminated locally */
#define _BFD_PKT           20       /* BFD packet that needs to be 
                                       terminated locally */
#define _ICNM_PKT          21       /* ICNM packet that needs to be 
                                       terminated locally */
#define _1588_PKT          22       /* ICNM packet that needs to be 
                                       terminated locally */
#define _KNOWN_TRILL_PKT   23       /* Known EgressRbridgeTRILL transit pkt */
#define _UNKNOWN_TRILL_PKT 24       /* UnKnown EgressRbridgeTRILL transit pkt */
#define _KNOWN_NIV_PKT     25       /* Known DestinationVifDownstream pkt */
#define _UNKNOWN_NIV_PKT   26       /* UnKnown DestinationVifDownstream pkt */
#define _KNOWN_L2GRE_PKT   27       /* Known L2 Gre pkt */
#define _KNOWN_VXLAN_PKT   28       /* Known VXLAN pkt */
#define _KNOWN_FCOE_PKT    29       /* Known FCoE pkt */
#define _UNKNOWN_FCOE_PKT  30       /* UnKnown FCoE pkt */



#define _UNKNOWN_PKT_KATANA       0x00       /* unknown packet */
#define _CONTROL_PKT_KATANA       0x01       /* pkt(12, 13) == 0x8808 */
#define _BPDU_PKT_KATANA          0x02       /* L2 USER ENTRY table BPDU bit */
#define _L2BC_PKT_KATANA          0x03       /* L2 Broadcast pkt */
#define _L2UC_PKT_KATANA          0x04       /* L2 Unicast pkt */
#define _L2DLF_PKT_KATANA         0x05       /* L2 destination lookup failure */
#define _UNKNOWN_IPMC_PKT_KATANA  0x06       /* Unknown IP Multicast pkt */
#define _KNOWN_IPMC_PKT_KATANA    0x07       /* Known IP Multicast pkt */
#define _KNOWN_L2MC_PKT_KATANA    0x08       /* Known L2 multicast pkt */
#define _UNKNOWN_L2MC_PKT_KATANA  0x09       /* Unknown L2 multicast pkt */
#define _KNOWN_L3UC_PKT_KATANA    0x0a       /* Known L3 Unicast pkt */
#define _UNKNOWN_L3UC_PKT_KATANA  0x0b       /* Unknown L3 Unicast pkt */
#define _KNOWN_MPLS_PKT_KATANA    0x0c       /* Known MPLS pkt */
#define _KNOWN_MPLS_L3_PKT_KATANA 0x0d       /* Known L3 MPLS pkt */
#define _KNOWN_MPLS_L2_PKT_KATANA 0x0e       /* Known L2 MPLS pkt */
#define _UNKNOWN_MPLS_PKT_KATANA  0x0f       /* Unknown MPLS pkt */
#define _KNOWN_MIM_PKT_KATANA     0x10       /* Known MIM pkt */
#define _UNKNOWN_MIM_PKT_KATANA   0x11       /* Unknown MIM pkt */
#define _KNOWN_MPLS_MULTICAST_PKT_KATANA    0x12  /* Known MPLS multicast pkt */

#define _UNKNOWN_PKT_TR3       0       /* unknown packet */
#define _CONTROL_PKT_TR3       1       /* pkt(12, 13) == 0x8808 */
#define _OAM_PKT_TR3           2       /* OAM packet            */
#define _BFD_PKT_TR3           3       /* BFD packet            */
#define _BPDU_PKT_TR3          4       /* L2 USER ENTRY table BPDU bit */
#define _ICNM_PKT_TR3          5       /* ICNM packet            */
#define _1588_PKT_TR3          6       /* 1588 packet            */
#define _KNOWN_L2UC_PKT_TR3    8       /* Known destination L2 unicast packet*/
#define _L2UC_PKT_TR3          _KNOWN_L2UC_PKT_TR3
                                       /* L2UC_PKT_TR3: L2 Unicast pkt */
#define _UNKNOWN_L2UC_PKT_TR   9       /* Unknown destination L2 UnicastPacket*/
#define _L2DLF_PKT_TR3         _UNKNOWN_L2UC_PKT_TR
                                       /* L2 destination lookup failure */
#define _KNOWN_L2MC_PKT_TR3    10       /* Known L2 multicast pkt */
#define _UNKNOWN_L2MC_PKT_TR3  11       /* Unknown L2 multicast pkt */
#define _L2BC_PKT_TR3          12      /* L2 Broadcast pkt */
#define _KNOWN_L3UC_PKT_TR3    16       /* Known L3 Unicast pkt */
#define _UNKNOWN_L3UC_PKT_TR3  17       /* Unknown L3 Unicast pkt */
#define _KNOWN_IPMC_PKT_TR3    18       /* Known IP Multicast pkt */
#define _UNKNOWN_IPMC_PKT_TR3  19       /* Unknown IP Multicast pkt */
#define _KNOWN_MPLS_L2_PKT_TR3 24       /* Known L2 MPLS pkt */
#define _UNKNOWN_MPLS_PKT_TR3  25       /* Unknown MPLS pkt */
#define _KNOWN_MPLS_L3_PKT_TR3 26       /* Known L3 MPLS pkt */
#define _KNOWN_MPLS_PKT_TR3    28       /* Known MPLS pkt */
#define _KNOWN_MPLS_MULTICAST_PKT_TR3    29       /* Known MPLS multicast pkt */
#define _KNOWN_MIM_PKT_TR3     32       /* Known MIM pkt */
#define _UNKNOWN_MIM_PKT_TR3   33       /* Unknown MIM pkt */
#define _KNOWN_TRILL_PKT_TR3   40       /* Known EgressRbridgeTRILL Transit 
                                           pkt */
#define _UNKNOWN_TRILL_PKT_TR3 41       /* UnKnown EgressRbridgeTRILL Transit
                                           Pkt */
#define _KNOWN_NIV_PKT_TR3     48       /* Known DestinationVifDownstream pkt */
#define _UNKNOWN_NIV_PKT_TR3   49       /* UnKnown DestinationVifDownstream 
                                           pkt */
/* added in TD2 */
#define _KNOWN_L2GRE_PKT_TD2   50       /* Known L2 Gre pkt */
#define _KNOWN_VXLAN_PKT_TD2   51       /* Known VXLAN pkt */
#define _KNOWN_FCOE_PKT_TD2    52       /* Known FCoE pkt */
#define _UNKNOWN_FCOE_PKT_TD2  53       /* Unknown FCoE pkt */

/* Egress packet types */
#define _L2_UC_PKT_EGR       0       /* Egr L2 UC pkt */
#define _L2_NON_UC_PKT_EGR   1       /* Egr L2 Non UC pkt */


#define BCM_FLEX_INGRESS_POOL               0x1
#define BCM_FLEX_EGRESS_POOL                0x2
#define BCM_FLEX_ALLOCATE_POOL              0x4
#define BCM_FLEX_DEALLOCATE_POOL            0x8
#define BCM_FLEX_SHARED_POOL                0x10
#define BCM_FLEX_NOT_SHARED_POOL            0x20

/* For internal use only */
typedef enum bcm_int_stat_object_e {
    bcmIntStatObjectIngAgm = bcmStatObjectMaxValue,
    bcmIntStatObjectIngAgmSecondLookup,
    bcmIntStatObjectEgrLatencyMonitor0,
    bcmIntStatObjectEgrLatencyMonitor1,
    bcmIntStatObjectEgrLatencyMonitor2,
    bcmIntStatObjectEgrLatencyMonitor3,
    bcmIntStatObjectMaxValue
} bcm_int_stat_object_t;

typedef enum bcm_int_stat_group_mode_e {
    bcmIntStatGroupModeLatencyMonitor = bcmStatGroupModeCount
} bcm_int_stat_group_mode_t;

typedef enum bcm_int_stat_group_mode_hint_type_e {
    bcmIntStatGroupAllocHintIngressAgmGroup = bcmStatGroupAllocHintCount, 
    bcmIntStatGroupAllocHintCount
} bcm_int_stat_group_mode_hint_type_t;

typedef struct  bcm_stat_flex_pool_stat_s {
    uint32  used_by_tables;
    SHR_BITDCL used_by_objects[_SHR_BITDCLSIZE(bcmIntStatObjectMaxValue)];
    uint32  used_entries;
    uint32  attached_entries;
} bcm_stat_flex_pool_stat_t;

#define BCM_FLEX_POOL_GROUP_USED_BY_NONE  0
#define BCM_FLEX_POOL_GROUP_USED_BY_OAM   1
#define BCM_FLEX_POOL_GROUP_USED_BY_OTHER 2

typedef struct  bcm_stat_flex_pool_group_s {
    uint32  group_type;
    uint32  free_pools;
    uint32  base_pool;
}bcm_stat_flex_pool_group_t;



/* ******************************************************************* */
/* counter-id-specific reference count will be added later i.e. after
   basic functionality starts working */
/* ******************************************************************* */
typedef struct  bcm_stat_flex_ingress_mode_s {
    uint32        available;
    uint32        reference_count;
    uint32        total_counters;
    bcm_stat_group_mode_t    group_mode;
    uint32                              flags;
    uint32                              oam_flags;
    uint32                              num_selectors;
    bcm_stat_group_mode_attr_selector_t *attr_selectors;
    bcm_stat_flex_ing_attr_t ing_attr;
}bcm_stat_flex_ingress_mode_t;

#define BCM_CUSTOM_INGRESS_MODE_START (16)
#define BCM_CUSTOM_INGRESS_MODE_MAX_COUNT (152*1024)
#define BCM_STAT_FLEX_CUSTOM_INGRESS_COUNTER_MAX_MODE (BCM_CUSTOM_INGRESS_MODE_MAX_COUNT + BCM_CUSTOM_INGRESS_MODE_START) /*152K custom modes to handle maximum possible */

#define BCM_CUSTOM_EGRESS_MODE_START (BCM_STAT_FLEX_CUSTOM_INGRESS_COUNTER_MAX_MODE)
#define BCM_CUSTOM_EGRESS_MODE_MAX_COUNT (32*1024)
#define BCM_STAT_FLEX_CUSTOM_EGRESS_COUNTER_MAX_MODE (BCM_CUSTOM_EGRESS_MODE_START + BCM_CUSTOM_EGRESS_MODE_MAX_COUNT)

typedef struct _bcm_stat_counter_map {
	uint32                mode;
	uint32                pool;
	uint32                base_idx;
    bcm_stat_object_t     object;
    bcm_stat_group_mode_t group;
    uint8                 pipe;
	int                   used;
}bcm_stat_counter_map_t;

/*"Wrapper" Modes for custom IFP stats*/
typedef struct  bcm_stat_flex_custom_ingress_mode_s {
    uint8        used;
    uint8        offset_mode;
    int8         AllocPool; /* Pool actually allocated for IFP. Group ID cannot identify the exact pool */
    bcm_stat_group_mode_hint_t hint; 
    uint32                              num_selectors;
    bcm_stat_group_mode_attr_selector_t *attr_selectors;
    uint32        total_counters;
}bcm_stat_flex_custom_ingress_mode_t;

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
/*"Wrapper" Modes for custom EFP stats*/
typedef struct  bcm_stat_flex_custom_egress_mode_s {
    uint8        used;
    uint8        offset_mode;
    int8         AllocPool;
    bcm_stat_group_mode_hint_t hint;
    uint32                              num_selectors;
    bcm_stat_group_mode_attr_selector_t *attr_selectors;
    uint32        total_counters;
}bcm_stat_flex_custom_egress_mode_t;

#ifdef BCM_WARM_BOOT_SUPPORT
typedef struct bcm_stat_flex_custom_mode_info_s {
    uint32 custom_mode;
    uint8  offset_mode;
    uint8  AllocPool;
    uint32 fg_id; /*field group id - 0xffffffff means invalid */
}bcm_stat_flex_custom_mode_info_t;

typedef struct bcm_stat_flex_counter_id_info_s {
    uint32 mode;
    uint8  pool;
    uint8  used;
    uint32 base_idx;
    uint16 object;
    uint16 group;
}bcm_stat_flex_counter_id_info_t;
#endif
#endif

typedef struct  bcm_stat_flex_egress_mode_s {
    uint32        available;
    uint32        reference_count;
    uint32        total_counters;
    uint32        group_mode;
    uint32                              flags;
    uint32                              num_selectors;
    bcm_stat_group_mode_attr_selector_t *attr_selectors;
    bcm_stat_flex_egr_attr_t egr_attr;
}bcm_stat_flex_egress_mode_t;

typedef struct bcm_stat_flex_table_info_s {
    soc_mem_t                 table;
    uint32                    index;
    bcm_stat_flex_direction_t direction;
}bcm_stat_flex_table_info_t;

typedef struct  bcm_stat_flex_pool_attribute_t {
        int    module_type;
        int    pool_size;
        uint32 flags ;
        int    pool_id;
        int    offset;
}bcm_stat_flex_pool_attribute_t;

typedef enum bcm_stat_flex_group_mode_e {
   bcmStatGroupModeFlex1 = bcmStatGroupModeCount,
   bcmStatGroupModeFlex2, 
   bcmStatGroupModeFlex3, 
   bcmStatGroupModeFlex4 
}bcm_stat_flex_group_mode_t;

/* BEGIN: NEW FLEX FEATURE RELATED STUFF */
#define BCM_STAT_FLEX_MAX_COUNTER 256
#define BCM_STAT_FLEX_MAX_SELECTORS 512

#define SHR_BITWID 32
#define BCM_STAT_FLEX_VALUE_SET(_array, _value)    \
        (((_array)[(_value) / SHR_BITWID]) |= (1U << ((_value) % SHR_BITWID)))

#define BCM_STAT_FLEX_VALUE_GET(_array, _value)    \
        (((_array)[(_value) / SHR_BITWID]) & (1U << ((_value) % SHR_BITWID)))

#define BCM_STAT_FLEX_BIT_ARRAY_SIZE 32

#define BCM_STAT_FLEX_DETACH_RV_SET(rv, _array)                \
    if (BCM_E_NONE == (_array)[bcmStatFlexDirectionIngress]) { \
        if (BCM_E_NOT_FOUND ==                                 \
            (_array)[bcmStatFlexDirectionEgress]) {            \
            rv = BCM_E_NONE;                                   \
        } else {                                               \
            rv = (_array)[bcmStatFlexDirectionEgress];         \
        }                                                      \
    } else if (BCM_E_NOT_FOUND ==                              \
                (_array)[bcmStatFlexDirectionIngress]){        \
        rv = (_array)[bcmStatFlexDirectionEgress];             \
    } else {                                                   \
        rv = (_array)[bcmStatFlexDirectionIngress];            \
    }


#define BCM_STAT_FLEX_DETACH_RETURN(_array)                    \
    if (BCM_E_NONE == (_array)[bcmStatFlexDirectionIngress]) { \
        if (BCM_E_NOT_FOUND ==                                 \
            (_array)[bcmStatFlexDirectionEgress]) {            \
            return BCM_E_NONE;                                 \
        } else {                                               \
            return (_array)[bcmStatFlexDirectionEgress];       \
        }                                                      \
    } else if (BCM_E_NOT_FOUND ==                              \
                (_array)[bcmStatFlexDirectionIngress]){        \
        return (_array)[bcmStatFlexDirectionEgress];           \
    } else {                                                   \
        return (_array)[bcmStatFlexDirectionIngress];          \
    }

typedef enum bcm_stat_flex_attr_with_value_e {
              bcmStatFlexAttrPort = 0,
              bcmStatFlexAttrTosDscp = 1,
              bcmStatFlexAttrTosEcn = 2,
              bcmStatFlexAttrSvp = 3,
              bcmStatFlexAttrDvp = 4,
              bcmStatFlexAttrUdf = 5,
              bcmStatFlexAttrComb = 6,
              bcmStatFlexAttrMax = 7
}bcm_stat_flex_attr_with_value_t;

#define BCM_MAX_IFP_COLOR_COUNTERS 3
#define BCM_MAX_IFP_COLORS         3

typedef struct _bcm_ifp_color_map_s {
    uint8 map[BCM_MAX_IFP_COLOR_COUNTERS];
} _bcm_ifp_color_map_t;

typedef struct bcm_stat_flex_combine_attr_counter_s { 
       uint32 fc_type;

#define BCM_STAT_FLEX_TCP_FLAG_TYPE_FIN                   0x00000001
#define BCM_STAT_FLEX_TCP_FLAG_TYPE_SYN                   0x00000002
#define BCM_STAT_FLEX_TCP_FLAG_TYPE_RST                   0x00000004
#define BCM_STAT_FLEX_TCP_FLAG_TYPE_PSH                   0x00000008
#define BCM_STAT_FLEX_TCP_FLAG_TYPE_ACK                   0x00000010
#define BCM_STAT_FLEX_TCP_FLAG_TYPE_URG                   0x00000020
#define BCM_STAT_FLEX_TCP_FLAG_TYPE_ECE                   0x00000040
#define BCM_STAT_FLEX_TCP_FLAG_TYPE_CWR                   0x00000080
       uint32 tcp_flags;

#define BCM_STAT_FLEX_INT_CN0                             0x00000001
#define BCM_STAT_FLEX_INT_CN1                             0x00000002
#define BCM_STAT_FLEX_INT_CN2                             0x00000004
#define BCM_STAT_FLEX_INT_CN3                             0x00000008
       uint32 int_cn;

#define BCM_STAT_FLEX_COS0                                0x00000001
#define BCM_STAT_FLEX_COS1                                0x00000002
#define BCM_STAT_FLEX_COS2                                0x00000004
#define BCM_STAT_FLEX_COS3                                0x00000008
#define BCM_STAT_FLEX_COS4                                0x00000010
#define BCM_STAT_FLEX_COS5                                0x00000020
#define BCM_STAT_FLEX_COS6                                0x00000040
#define BCM_STAT_FLEX_COS7                                0x00000080
#define BCM_STAT_FLEX_COS8                                0x00000100
#define BCM_STAT_FLEX_COS9                                0x00000200
#define BCM_STAT_FLEX_COS10                               0x00000400
#define BCM_STAT_FLEX_COS11                               0x00000800
#define BCM_STAT_FLEX_COS12                               0x00001000
#define BCM_STAT_FLEX_COS13                               0x00002000
#define BCM_STAT_FLEX_COS14                               0x00004000
#define BCM_STAT_FLEX_COS15                               0x00008000
        uint32 mmu_cos_flags;

#define BCM_STAT_FLEX_UC_QUEUEING_ENABLE              0x00000001
#define BCM_STAT_FLEX_UC_QUEUEING_DISABLE             0x00000002
        uint32 uc_queueing_flags;

#define BCM_STAT_FLEX_CONGESTION_MARKED_ENABLE              0x00000001
#define BCM_STAT_FLEX_CONGESTION_MARKED_DISABLE             0x00000002
        uint32 congestion_marked_flags;

#define BCM_STAT_FLEX_COLOR_GREEN                     0x00000001
#define BCM_STAT_FLEX_COLOR_YELLOW                    0x00000002
#define BCM_STAT_FLEX_COLOR_RED                       0x00000004
        uint32 pre_ifp_color_flags; 
        uint32 ifp_color_flags; 
        uint32 pre_ifp_color_mpls_label3_flags;
        uint32 pre_ifp_color_mpls_label2_flags;
        uint32 pre_ifp_color_mpls_label1_flags;

#define BCM_STAT_FLEX_PRI0                                0x00000001
#define BCM_STAT_FLEX_PRI1                                0x00000002
#define BCM_STAT_FLEX_PRI2                                0x00000004
#define BCM_STAT_FLEX_PRI3                                0x00000008
#define BCM_STAT_FLEX_PRI4                                0x00000010
#define BCM_STAT_FLEX_PRI5                                0x00000020
#define BCM_STAT_FLEX_PRI6                                0x00000040
#define BCM_STAT_FLEX_PRI7                                0x00000080
#define BCM_STAT_FLEX_PRI8                                0x00000100
#define BCM_STAT_FLEX_PRI9                                0x00000200
#define BCM_STAT_FLEX_PRI10                               0x00000400
#define BCM_STAT_FLEX_PRI11                               0x00000800
#define BCM_STAT_FLEX_PRI12                               0x00001000
#define BCM_STAT_FLEX_PRI13                               0x00002000
#define BCM_STAT_FLEX_PRI14                               0x00004000
#define BCM_STAT_FLEX_PRI15                               0x00008000
        uint32 int_pri_flags; 
        uint32 int_pri_mpls_label3_flags;
        uint32 int_pri_mpls_label2_flags;
        uint32 int_pri_mpls_label1_flags;

#define BCM_STAT_FLEX_VLAN_FORMAT_UNTAGGED                    0x00000001
#define BCM_STAT_FLEX_VLAN_FORMAT_INNER                       0x00000002
#define BCM_STAT_FLEX_VLAN_FORMAT_OUTER                       0x00000004
#define BCM_STAT_FLEX_VLAN_FORMAT_BOTH                        0x00000008
        uint32 vlan_format_flags; 

/* #define BCM_STAT_FLEX_PRI0..7 */
        uint32 outer_dot1p_flags; 
        uint32 inner_dot1p_flags; 

#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_PKT                    0x00000001
#define BCM_STAT_FLEX_PKT_TYPE_CONTROL_PKT                    0x00000002
#define BCM_STAT_FLEX_PKT_TYPE_OAM_PKT                        0x00000004
#define BCM_STAT_FLEX_PKT_TYPE_BFD_PKT                        0x00000008
#define BCM_STAT_FLEX_PKT_TYPE_BPDU_PKT                       0x00000010
#define BCM_STAT_FLEX_PKT_TYPE_ICNM_PKT                       0x00000020
#define BCM_STAT_FLEX_PKT_TYPE_PKT_IS_1588                    0x00000040
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_L2UC_PKT                 0x00000080
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_L2UC_PKT               0x00000100
#define BCM_STAT_FLEX_PKT_TYPE_L2BC_PKT                       0x00000200
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_L2MC_PKT                 0x00000400
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_L2MC_PKT               0x00000800
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_L3UC_PKT                 0x00001000
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_L3UC_PKT               0x00002000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_IPMC_PKT                 0x00004000
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_IPMC_PKT               0x00008000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_MPLS_L2_PKT              0x00010000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_MPLS_L3_PKT              0x00020000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_MPLS_PKT                 0x00040000
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_MPLS_PKT               0x00080000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_MPLS_MULTICAST_PKT       0x00100000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_MIM_PKT                  0x00200000
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_MIM_PKT                0x00400000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_TRILL_PKT                0x00800000
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_TRILL_PKT              0x01000000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_NIV_PKT                  0x02000000
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_NIV_PKT                0x04000000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_L2GRE_PKT                0x08000000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_VXLAN_PKT                0x10000000
#define BCM_STAT_FLEX_PKT_TYPE_KNOWN_FCOE_PKT                 0x20000000
#define BCM_STAT_FLEX_PKT_TYPE_UNKNOWN_FCOE_PKT               0x40000000

        uint32 pkt_resolution_flags;

        /* These new defines go into pkt_resolution_high_flags */        
#define BCM_STAT_FLEX_PKT_TYPE_2_L2_UC_PKT                    0x00000001
#define BCM_STAT_FLEX_PKT_TYPE_2_L2_NON_UC_PKT                0x00000002
        
        uint32 pkt_resolution_high_flags;

#define BCM_STAT_FLEX_DROP_ENABLE                             0x00000001
#define BCM_STAT_FLEX_DROP_DISABLE                            0x00000002
        uint32 drop_flags; 
#define BCM_STAT_FLEX_IP_PKT_ENABLE                           0x00000001
#define BCM_STAT_FLEX_IP_PKT_DISABLE                          0x00000002
        uint32 ip_pkt_flags;
        /* Bit Value array for port,tos,svp & dvp considering max value as 1024
        =uint32=32 bits *                 size=BCM_STAT_FLEX_BIT_ARRAY_SIZE)*/
        uint32 value_array[bcmStatFlexAttrMax][BCM_STAT_FLEX_BIT_ARRAY_SIZE];

}bcm_stat_flex_combine_attr_counter_t ;

typedef struct bcm_stat_flex_attribute_s {
        uint32 fc_type;
        uint32 tcp_flags;
        uint32 int_cn;
        uint32 mmu_cos;
        uint32 uc_queueing;
        uint32 congestion_marked;
        uint32 cng_3;
        uint32 phb_3;
        uint32 cng_2;
        uint32 phb_2;
        uint32 cng_1;
        uint32 phb_1;
        uint32 pre_ifp_color; 
        uint32 ifp_color;     
        uint32 int_pri;    
        uint32 vlan_format;
        uint32 outer_dot1p;  
        uint32 inner_dot1p;  
        uint32  port;
        uint32  tos_dscp;
        uint32  tos_ecn;
        uint32 pkt_resolution;  
        uint32 svp;
        uint32 dvp;
        uint32 drop;
        uint32 ip_pkt;
        uint32 udf;
        uint32 udf_bitmap;
        uint32 subport;
        uint32 total_counters;
        uint32 total_selectors;
        uint32 combine_mmu_cos_flags;
        uint32 combine_uc_queueing_flags;
        uint32 combine_congestion_marked_flags;
        uint32 combine_pre_ifp_color_flags;
        uint32 combine_ifp_color_flags;
        uint32 combine_int_pri_flags;
        uint32 combine_vlan_format_flags;
        uint32 combine_outer_dot1p_flags;
        uint32 combine_inner_dot1p_flags;
        uint32 combine_pkt_resolution_flags;
        uint32 combine_drop_flags;
        uint32 combine_ip_pkt_flags;
        uint32 combine_value_array[bcmStatFlexAttrMax][BCM_STAT_FLEX_BIT_ARRAY_SIZE];
        bcm_stat_flex_combine_attr_counter_t  *combine_attr_counter;
        /* combine_pkt_resolution_flags is expanded with 'high below */
        uint32 combine_pkt_resolution_high_flags;
}bcm_stat_flex_attribute_t;

typedef enum pkt_cmprsd_attr_map_e {
    ing_pkt_attr_pri_cng_map = 0,    /* ING_FLEX_CTR_PRI_CNG_MAPm*/
    ing_pkt_attr_pkt_pri_map = 1,    /* ING_FLEX_CTR_PKT_PRI_MAPm */
    ing_pkt_attr_port_map = 2,       /* ING_FLEX_CTR_PORT_MAPm */
    ing_pkt_attr_tos_map = 3,        /* ING_FLEX_CTR_TOS_MAPm */
    ing_pkt_attr_pkt_res_map = 4,    /* ING_FLEX_CTR_PKT_RES_MAPm */
    ing_pkt_attr_ip_pkt_map = 5,     /* no map for IP_PKT in h/w */
    egr_pkt_attr_pri_cng_map = 6,/* EGR_FLEX_CTR_PRI_CNG_MAP */
    egr_pkt_attr_pkt_pri_map = 7,/* EGR_FLEX_CTR_PKT_PRI_MAP */
    egr_pkt_attr_port_map = 8,   /* EGR_FLEX_CTR_PORT_MAPm  */
    egr_pkt_attr_tos_map = 9,    /* EGR_FLEX_CTR_TOS_MAPm */
    egr_pkt_attr_pkt_res_map = 10,/* EGR_FLEX_CTR_PKT_RES_MAPm */
    pkt_attr_map_count = 11
} pkt_cmprsd_attrs_map_t;

/* Structure for global compression table attribute selectors map */
typedef struct _bcm_stat_flex_compressed_mode_attr_sel_map_s {
    uint8  ref_counts[12];
    uint32 direction;
    uint32 combine_mmu_cos_flags;
    uint32 combine_uc_queueing_flags;
    uint32 combine_congestion_marked_flags;
    uint32 cng;
    uint32 ifp_cng;
    uint32 pre_ifp_color;
    uint32 ifp_color;
    uint32 combine_pre_ifp_color_flags;
    uint32 combine_ifp_color_flags;
    uint32 int_pri;
    uint32 combine_int_pri_flags;
    uint32 vlan_format;
    uint32 combine_vlan_format_flags;
    uint32 outer_dot1p;
    uint32 combine_outer_dot1p_flags;
    uint32 inner_dot1p;
    uint32 combine_inner_dot1p_flags;
    uint32 port;
    uint32 tos_dscp;
    uint32 tos_ecn;
    uint32 pkt_resolution;
    uint32 combine_pkt_resolution_flags;
    uint32 svp_type;
    uint32 dvp_type;
    uint32 drop;
    uint32 combine_drop_flags;
    uint32 ip_pkt;
    uint32 combine_ip_pkt_flags;
    uint32 combine_value_array[bcmStatFlexAttrMax][BCM_STAT_FLEX_BIT_ARRAY_SIZE];
    uint32 combine_pkt_resolution_high_flags;
} _bcm_stat_flex_compressed_mode_attr_sel_map_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/* Stat Flex Group Attribute Selector info that needs level 2 WB*/
typedef struct bcm_stat_group_mode_attr_selector_wb_s {
    uint32 counter_offset;              /* Counter Offset */
    bcm_stat_group_mode_attr_t attr;    /* Attribute Selector */
    uint32 attr_value;                  /* Attribute Values */
}bcm_stat_group_mode_attr_selector_wb_t;
typedef struct bcm_stat_flex_group_mode_related_info_s {
    uint32 flags;
    uint32 total_counters;
    uint32 num_selectors;
    bcm_stat_group_mode_attr_selector_wb_t attr_selectors
                                        [BCM_STAT_FLEX_MAX_SELECTORS];
}bcm_stat_flex_group_mode_related_info_t;

typedef struct bcm_stat_flex_group_mode_related_info_old_s {
    uint32 flags;
    uint32 total_counters;
    uint32 num_selectors;
    bcm_stat_group_mode_attr_selector_wb_t attr_selectors
                                        [256];
}bcm_stat_flex_group_mode_related_info_old_t;
typedef struct bcm_stat_flex_custom_mode_related_info_s {
    uint16 custom_mode;
    uint8  offset_mode;
    uint8  AllocPool;
    uint32 fg_id; /*field group id - 0xffffffff means invalid */
}bcm_stat_flex_custom_mode_related_info_t;

typedef struct bcm_stat_flex_counter_id_related_info_s {
    uint16 mode;
    uint8  pool;
    uint8  used; 
    uint32 base_idx;
    uint16 object;
    uint16 group;
}bcm_stat_flex_counter_id_related_info_t;

#endif
#define BCM_MAX_STAT_COUNTER_IDS (152*1024)

typedef struct bcm_stat_flex_flowtracker_pool_info_s {
    uint8  pool_id;
    uint32 start_idx;
    uint32 end_idx;
}bcm_stat_flex_flowtracker_pool_info_t;

typedef struct bcm_stat_flex_flowtracker_counter_info_s {
    uint8 offset_mode;
    uint8 pool_num;
    bcm_stat_flex_flowtracker_pool_info_t  pool_info[BCM_STAT_TH_FLEX_COUNTER_MAX_POOL];
}bcm_stat_flex_flowtracker_counter_info_t;


#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/esw/switch.h>
#define BCM_STAT_WB_VERSION_1_3                SOC_SCACHE_VERSION(1,3)
#define BCM_STAT_WB_VERSION_1_4                SOC_SCACHE_VERSION(1,4)
#define BCM_STAT_WB_VERSION_1_5                SOC_SCACHE_VERSION(1,5)
#define BCM_STAT_WB_VERSION_1_6                SOC_SCACHE_VERSION(1,6)
#define BCM_STAT_WB_VERSION_1_7                SOC_SCACHE_VERSION(1,7)
#define BCM_STAT_WB_VERSION_1_8                SOC_SCACHE_VERSION(1,8)
#define BCM_STAT_WB_VERSION_1_9                SOC_SCACHE_VERSION(1,9)
#define BCM_STAT_WB_VERSION_1_10               SOC_SCACHE_VERSION(1,10)
#define BCM_STAT_WB_VERSION_1_11               SOC_SCACHE_VERSION(1,11)
#define BCM_STAT_WB_VERSION_1_12               SOC_SCACHE_VERSION(1,12)
#define BCM_STAT_WB_DEFAULT_VERSION        BCM_STAT_WB_VERSION_1_12
#endif

/* END: NEW FLEX FEATURE RELATED STUFF */

extern bcm_error_t _bcm_esw_stat_group_create (
                   int                   unit,
                   bcm_stat_object_t     object,
                   bcm_stat_group_mode_t group_mode,
                   uint32                *stat_counter_id,
                   uint32                *num_entries);
extern bcm_error_t _bcm_esw_stat_group_destroy(
                   int    unit,
                   uint32 stat_counter_id);
extern bcm_error_t _bcm_esw_stat_flex_get_ingress_mode_info(
                   int                          unit,
                   bcm_stat_flex_mode_t         mode,
                   bcm_stat_flex_ingress_mode_t *ingress_mode);
extern bcm_error_t _bcm_esw_stat_flex_get_custom_ingress_mode_info(
            int                          unit,
            bcm_stat_flex_mode_t         mode,
            bcm_stat_flex_custom_ingress_mode_t *flex_ingress_mode);

extern bcm_error_t _bcm_esw_stat_flex_update_custom_ingress_mode_info(
            int                          unit,
            bcm_stat_flex_mode_t         mode,
            bcm_stat_flex_custom_ingress_mode_t *flex_ingress_mode);

extern bcm_error_t _bcm_esw_stat_flex_get_egress_mode_info(
                   int                         unit,
                   bcm_stat_flex_mode_t        mode,
                   bcm_stat_flex_egress_mode_t *egress_mode);
extern bcm_error_t _bcm_esw_stat_flex_get_available_mode(
                   int                       unit,
                   bcm_stat_flex_direction_t direction,
                   bcm_stat_flex_mode_t      *mode);
extern bcm_error_t _bcm_esw_stat_flex_update_selector_keys_enable_fields(
                   int       unit,
                   soc_reg_t pkt_attr_selector_key_reg,
                   uint64    *pkt_attr_selector_key_reg_value,
                   uint32    ctr_pkt_attr_bit_position,
                   uint32    ctr_pkt_attr_total_bits,
                   uint8     pkt_attr_field_mask_v,
                   uint8     *ctr_current_bit_selector_position);
extern bcm_error_t _bcm_esw_stat_flex_update_offset_table(
                   int                                unit,
                   bcm_stat_flex_direction_t          direction,
                   soc_mem_t                          flex_ctr_offset_table_mem,
                   bcm_stat_flex_mode_t               mode,
                   uint32                             total_counters,
                   bcm_stat_flex_offset_table_entry_t offset_table_map[256]);
extern bcm_error_t _bcm_esw_stat_flex_egress_reserve_mode(
                   int                      unit,
                   bcm_stat_flex_mode_t     mode,
                   uint32                   total_counters,
                   bcm_stat_flex_egr_attr_t *egr_attr);
extern bcm_error_t _bcm_esw_stat_flex_ingress_reserve_mode(
                   int                      unit,
                   bcm_stat_flex_mode_t     mode,
                   uint32                   total_counters,
                   bcm_stat_flex_ing_attr_t *ing_attr);
extern bcm_error_t _bcm_esw_stat_flex_unreserve_mode(
                   int                       unit,
                   bcm_stat_flex_direction_t direction,
                   bcm_stat_flex_mode_t      mode);
extern bcm_error_t _bcm_esw_stat_flex_unreserve_flags(
                   int                       unit,
                   bcm_stat_flex_mode_t      mode,
                   uint32                    flags);
extern 
bcm_error_t _bcm_esw_stat_flex_update_udf_selector_keys(
            int                                    unit,
            bcm_stat_flex_direction_t              direction,
            soc_reg_t                              pkt_attr_selector_key_reg,
            bcm_stat_flex_udf_pkt_attr_selectors_t *udf_pkt_attr_selectors,
            uint32                                 *total_udf_bits);
extern bcm_error_t _bcm_esw_stat_flex_init(int unit);
extern bcm_error_t _bcm_esw_stat_flex_cleanup(int unit);
extern bcm_error_t _bcm_esw_stat_flex_sync(int unit);
extern void _bcm_esw_stat_flex_callback(int unit);
#if defined(BCM_TOMAHAWK_SUPPORT)
extern bcm_error_t
bcm_flex_ctr_field_stat_types_get(int unit, uint32 flex_sid, 
                                   bcm_stat_group_mode_attr_selector_t *attr_selectors,
                                   uint32                              *num_selectors);
extern bcm_stat_counter_map_t *stat_counter_map[SOC_MAX_NUM_DEVICES]; /* Stat counter IDs start from 1 */

extern void 
_bcm_fill_stat_counter_map(int unit, int id, uint32 mode, uint32 pool, uint32 base_idx,
                        bcm_stat_object_t object, bcm_stat_group_mode_t group);
extern void 
_bcm_get_stat_counter_map(int unit, int id, bcm_stat_counter_map_t *stat_ctr_map);

#endif
#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
/* Need to remove it later */
extern int _bcm_tr2_subport_gport_used(int unit, bcm_gport_t port);
#endif

extern bcm_error_t _bcm_esw_stat_flex_create_egress_mode (
                   int                      unit,
                   bcm_stat_flex_egr_attr_t *egr_attr,
                   bcm_stat_flex_mode_t     *mode,
                   bcm_stat_flex_attribute_t  *flex_attribute);
extern bcm_error_t _bcm_esw_stat_flex_create_ingress_mode (
                   int                      unit,
                   bcm_stat_flex_ing_attr_t *ing_attr,
                   bcm_stat_flex_mode_t     *mode,
                   bcm_stat_flex_attribute_t  *flex_attribute);
extern bcm_error_t _bcm_esw_stat_flex_delete_egress_mode(
                   int                  unit,
                   bcm_stat_flex_mode_t mode);
extern bcm_error_t _bcm_esw_stat_flex_delete_ingress_flags(
                   int                  unit,
                   bcm_stat_flex_mode_t mode,
                   uint32               flags);
extern bcm_error_t _bcm_esw_stat_flex_delete_egress_flags(
                   int                  unit,
                   bcm_stat_flex_mode_t mode,
                   uint32               flags);
extern bcm_error_t _bcm_esw_stat_flex_delete_ingress_mode(
                   int                  unit,
                   bcm_stat_flex_mode_t mode);


extern bcm_error_t _bcm_esw_stat_flex_destroy_egress_table_counters(
                   int                  unit,
                   soc_mem_t            egress_table,
                   int                  lookup,
                   bcm_stat_object_t    object,
                   bcm_stat_flex_mode_t offset_mode,
                   uint32               base_idx,
                   uint32               pool_number);
extern bcm_error_t _bcm_esw_stat_flex_detach_egress_table_counters(
                   int       unit,
                   bcm_stat_object_t object_id,
                   soc_mem_t egress_table,
                   uint32    index);
extern bcm_error_t _bcm_esw_stat_flex_destroy_ingress_table_counters(
                   int                  unit,
                   soc_mem_t            ingress_table,
                   int                  lookup,
                   bcm_stat_object_t    object,
                   bcm_stat_flex_mode_t offset_mode,
                   uint32               base_idx,
                   uint32               pool_number);
extern bcm_error_t _bcm_esw_stat_flex_detach_ingress_table_counters1(
                   int                  unit,
                   soc_mem_t            ingress_table,
                   uint32               index,
                   bcm_stat_object_t    object);
extern bcm_error_t _bcm_esw_stat_flex_detach_ingress_table_counters(
                   int       unit,
                   soc_mem_t ingress_table,
                   uint32    index);
extern bcm_error_t _bcm_esw_stat_flex_detach_ingress_table_counters_sw(
                    int       unit,
                    soc_mem_t ingress_table,
                    uint32    index,
                    bcm_stat_flex_mode_t offset_mode,
                    uint32               base_idx,
                    uint32               pool_number);
extern bcm_error_t _bcm_esw_stat_flex_attach_egress_table_counters1(
                   int                  unit,
                   soc_mem_t            egress_table,
                   uint32               index,
                   bcm_stat_flex_mode_t mode,
                   uint32               base_idx,
                   uint32               pool_number,
                   void                 *egress_entry_data1);
extern bcm_error_t _bcm_esw_stat_flex_attach_egress_table_counters(
                   int                  unit,
                   soc_mem_t            egress_table,
                   uint32               index,
                   bcm_stat_object_t    object_id,
                   bcm_stat_flex_mode_t mode,
                   uint32               base_idx,
                   uint32               pool_number);
extern bcm_error_t _bcm_esw_stat_flex_create_egress_table_counters(
                   int                  unit,
                   soc_mem_t            egress_table,
                   int                  lookup,
                   bcm_stat_object_t    object,
                   bcm_stat_flex_mode_t mode,
                   uint32               *base_idx,
                   uint32               *pool_number);
extern bcm_error_t _bcm_esw_stat_flex_attach_ingress_table_counters_update(
                   int                  unit,
                   uint32               pool_number,
                   uint32               pipe_num,
                   uint32               base_idx,
                   bcm_stat_flex_mode_t mode);  
extern bcm_error_t _bcm_esw_stat_flex_detach_ingress_table_counters_update(
                   int                  unit,
                   uint32               pool_number,
                   uint32               base_idx,
                   bcm_stat_flex_mode_t mode);  
extern bcm_error_t _bcm_esw_stat_flex_attach_ingress_table_counters2(
                   int                  unit,
                   soc_mem_t            ingress_table,
                   uint32               index,
                   bcm_stat_flex_mode_t mode,
                   uint32               base_idx,
                   uint32               pool_number,
                   bcm_stat_object_t    object,
                   void                 *ingress_entry_data1);
extern bcm_error_t _bcm_esw_stat_flex_attach_ingress_table_counters1(
                   int                  unit,
                   soc_mem_t            ingress_table,
                   uint32               index,
                   bcm_stat_flex_mode_t mode,
                   uint32               base_idx,
                   uint32               pool_number,
                   void                 *ingress_entry_data1);
extern bcm_error_t _bcm_esw_stat_flex_attach_ingress_table_counters(
                   int                  unit,
                   soc_mem_t            ingress_table,
                   uint32               index,
                   bcm_stat_flex_mode_t mode,
                   uint32               base_idx,
                   uint32               pool_number);
extern bcm_error_t _bcm_esw_stat_flex_create_ingress_table_counters(
                   int                  unit,
                   soc_mem_t            ingress_table,
                   int                  lookup,
                   bcm_stat_object_t    object,
                   bcm_stat_flex_mode_t mode,
                   uint32               *base_idx,
                   uint32               *pool_number);
extern bcm_error_t _bcm_esw_stat_counter_get(
                   int              unit,
                   int              sync_mode,
                   uint32           index,
                   soc_mem_t        table,
                   bcm_stat_object_t object,
                   uint32           byte_flag,
                   uint32           counter_index,
                   bcm_stat_value_t *value);
extern bcm_error_t _bcm_esw_stat_counter_set(
                   int              unit,
                   uint32           index,
                   soc_mem_t        table,
                   bcm_stat_object_t object,
                   uint32           byte_flag,
                   uint32           counter_index,
                   bcm_stat_value_t *value);
extern bcm_error_t _bcm_esw_stat_counter_raw_get(
                   int              unit,
                   int              sync_mode,
                   uint32           stat_counter_id,
                   uint32           byte_flag,
                   uint32           counter_index,
                   bcm_stat_value_t *value);
extern bcm_error_t _bcm_esw_stat_counter_raw_set(
            int              unit,
            uint32           stat_counter_id,
            uint32           byte_flag,
            uint32           counter_index,
            bcm_stat_value_t *value);
extern bcm_error_t _bcm_esw_stat_flex_get_table_info(
                   int                       unit,
                   bcm_stat_object_t         object,
                   uint32                    expected_num_tables,
                   uint32                    *actual_num_tables,
                   soc_mem_t                 *table,
                   bcm_stat_flex_direction_t *direction);
extern bcm_error_t _bcm_esw_stat_flex_reset_group_mode(
                   int                       unit,
                   bcm_stat_flex_direction_t direction,
                   uint32                    offset_mode,
                   bcm_stat_group_mode_t     group_mode);
extern bcm_error_t _bcm_esw_stat_flex_set_group_mode(
                   int                       unit,
                   bcm_stat_flex_direction_t direction,
                   uint32                    offset_mode,
                   bcm_stat_group_mode_t     group_mode);
extern void  _bcm_esw_stat_flex_show_mode_info(int unit); 
extern bcm_error_t _bcm_esw_stat_flex_get_ingress_object(
                   int               unit,
                   soc_mem_t         ingress_table,
                   uint32            table_index,
                   void              *ingress_entry,
                   bcm_stat_object_t *object);
extern bcm_error_t _bcm_esw_stat_flex_get_egress_object(
                   int               unit,
                   soc_mem_t         egress_table,
                   uint32            table_index,
                   void              *ingress_entry,
                   bcm_stat_object_t *object);
extern bcm_error_t _bcm_esw_stat_flex_get_counter_id(
                   int                        unit,
                   uint32                     num_of_tables,
                   bcm_stat_flex_table_info_t *table_info,
                   uint32                     *num_stat_counter_ids,
                   uint32                     *stat_counter_id);
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined (INCLUDE_L3)
extern bcm_error_t _bcm_esw_stat_flex_get_multi_counter_id(
                   int                        unit,
                   bcm_stat_flex_table_info_t *table_info,
                   int                        *num_stat_counter_ids,
                   uint32                     *stat_counter_id);
#endif
extern void _bcm_esw_stat_group_dump_info(
            int unit,
            int all_flag,
            bcm_stat_object_t object,
            bcm_stat_group_mode_t group);
extern void _bcm_esw_stat_get_counter_id(
            int                   unit,
            bcm_stat_group_mode_t group,
            bcm_stat_object_t     object,
            uint32                mode,
            uint32                pool_number,
            uint32                base_idx,
            uint32                *stat_counter_id);
extern void _bcm_esw_stat_counter_id_retrieve(
            int                   unit,
            bcm_stat_group_mode_t group,
            bcm_stat_object_t     object,
            uint32                mode,
            uint32                pool_number,
            uint32                base_idx,
            uint32                *stat_counter_id);
extern void _bcm_esw_stat_get_counter_id_info(
            int                   unit,
            uint32                stat_counter_id,
            bcm_stat_group_mode_t *group,
            bcm_stat_object_t     *object,
            uint32                *mode,
            uint32                *pool_number,
            uint32                *base_idx);

extern bcm_error_t _bcm_esw_stat_id_validate(int unit, uint32 stat_counter_id);

extern bcm_error_t _bcm_esw_stat_total_counters_get(int unit, uint32  stat_counter_id, uint32 *total_counters);

extern bcm_error_t _bcm_flex_ctr_field_stat_types_get(int unit, uint32 flex_sid,
                            bcm_stat_group_mode_attr_selector_t *attr_selectors,
                                                      uint32 *num_selectors);

extern void _bcm_esw_stat_flex_init_pkt_attr_bits(int unit);
extern void _bcm_esw_stat_flex_init_pkt_res_fields(int unit);
extern uint32 _bcm_esw_stat_flex_get_pkt_res_value(
              int unit,
              uint32 pkt_res_field);
extern bcm_error_t _bcm_esw_get_flex_counter_fields_values(int unit, 
                                                           uint32 index,
                                                           soc_mem_t table,
                                                           void *data,
                                                           bcm_stat_object_t object,
                                                           uint32 *offset_mode,
                                                           uint32 *pool_number,
                                                           uint32 *base_idx);
extern bcm_error_t _bcm_esw_set_flex_counter_fields_values(int       unit,
                                                           uint32    index, 
                                                           soc_mem_t table,
                                                           void      *data,
                                                           bcm_stat_object_t object,
                                                           uint32    offset_mode,
                                                           uint32    pool_number,
                                                           uint32    base_idx
);
extern bcm_error_t _bcm_esw_stat_validate_object(
                   int                       unit,
                   bcm_stat_object_t         object,
                   bcm_stat_flex_direction_t *direction);
extern bcm_error_t _bcm_esw_stat_validate_group(
                   int                   unit,
                   bcm_stat_group_mode_t group);
extern bcm_error_t _bcm_esw_stat_flex_pool_operation(
                   int                            unit,
                   bcm_stat_flex_pool_attribute_t *flex_pool_attribute);

extern bcm_error_t _bcm_esw_stat_id_get_all( int unit, bcm_stat_object_t object,
                                     int stat_max, uint32 *stat_array,
                                     int *stat_count);

extern bcm_error_t _bcm_esw_stat_object_pool_num_get( int unit,
                                 bcm_stat_object_t object, uint32 *pool_num);

/* Retrieves the flex counter details for a given direction and pipe */
extern int _bcm_esw_stat_flex_pipe_pool_info_multi_get(
    int unit,
    bcm_stat_flex_direction_t direction,
    uint32 pipe_num,
    uint32 num_pools,
    bcm_stat_flex_pool_stat_info_t *flex_pool_stat_info,
    uint32 *actual_num_pools);

extern int _bcm_esw_stat_flex_pool_info_multi_get(
    int unit,
    bcm_stat_flex_direction_t direction,
    uint32 num_pools,
    bcm_stat_flex_pool_stat_info_t *flex_pool_stat,
    uint32 *actual_num_pools);
/* Create Customized Stat Group mode for given Counter Attributes */
extern int _bcm_esw_stat_group_mode_id_create(
    int unit,
    uint32 flags,
    uint32 total_counters,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_selectors,
    uint32 *mode_id);

/* Retrieves Customized Stat Group mode Attributes for given mode_id */
extern int _bcm_esw_stat_group_mode_id_get(
    int unit,
    uint32 mode_id,
    uint32 *flags,
    uint32 *total_counters,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_selectors,
    uint32 *actual_num_selectors);

/* Destroys Customized Group mode */
extern int _bcm_esw_stat_group_mode_id_destroy(
    int unit,
    uint32 mode_id);

/* Associate an accounting object to customized group mode */
extern int _bcm_esw_stat_custom_group_create(
    int unit,
    uint32 mode_id,
    bcm_stat_object_t object,
    uint32 *stat_counter_id,
    uint32 *num_entries);

extern int _bcm_esw_stat_group_mode_id_config_create(
    int unit,
    uint32 options,
    bcm_stat_group_mode_id_config_t *stat_config,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_sel,
    uint32 *mode_id);

bcm_error_t 
_bcm_esw_stat_group_mode_id_config_get(
    int unit,
    uint32 mode_id,
    bcm_stat_group_mode_id_config_t *stat_config,
    uint32 num_selectors,
    bcm_stat_group_mode_attr_selector_t *attr_sel,
    uint32 *actual_num_selectors);

extern bcm_stat_flex_ing_pkt_attr_bits_t ing_pkt_attr_uncmprsd_bits_g[BCM_MAX_NUM_UNITS];
extern bcm_stat_flex_ing_pkt_attr_bits_t ing_pkt_attr_cmprsd_bits_g[BCM_MAX_NUM_UNITS];
extern bcm_stat_flex_egr_pkt_attr_bits_t egr_pkt_attr_uncmprsd_bits_g[BCM_MAX_NUM_UNITS];
extern bcm_stat_flex_egr_pkt_attr_bits_t egr_pkt_attr_cmprsd_bits_g[BCM_MAX_NUM_UNITS];
extern bcm_error_t _bcm_esw_stat_flex_update_ingress_flex_info(
            int                                 unit,
            bcm_stat_flex_mode_t                mode,
            uint32                              flags,
            uint32                              num_selectors,
            bcm_stat_group_mode_attr_selector_t *attr_selectors);
extern bcm_error_t _bcm_esw_stat_flex_update_egress_flex_info(
            int                                 unit,
            bcm_stat_flex_mode_t                mode,
            uint32                              flags,
            uint32                              num_selectors,
            bcm_stat_group_mode_attr_selector_t *attr_selectors);

extern void _bcm_esw_stat_flex_check_egress_mpls_tunnel_table(int unit);

extern
int _bcm_esw_stat_flex_compress_mode_id_compare(
        int                                       unit,
        bcm_stat_flex_direction_t                 direction,
        bcm_stat_flex_mode_t                      *mode,
        bcm_stat_flex_attribute_t  *flex_attribute);

extern
int _bcm_stat_flex_compressed_attr_tbl_write(int unit,
        bcm_stat_flex_direction_t direction,
        bcm_stat_flex_ing_cmprsd_attr_selectors_t *cmprsd_attr_selectors,
        bcm_stat_flex_egr_cmprsd_attr_selectors_t *egr_cmprsd_attr_selectors);
extern
int _bcm_stat_flex_compressed_attr_tbl_clear(int unit,
        bcm_stat_flex_direction_t direction,
        bcm_stat_flex_ing_cmprsd_attr_selectors_t *cmprsd_attr_selectors,
        bcm_stat_flex_egr_cmprsd_attr_selectors_t *egr_cmprsd_attr_selectors);

extern
int _bcm_esw_stat_flex_compressed_shared_attr_map_init(int unit,int alloc);

extern
int _bcm_esw_stat_flex_compressed_attr_flags_recover(int unit,
    uint8 **scache_position);

extern
int _bcm_esw_stat_flex_compressed_attr_flags_sync(int unit,
        uint8 **scache_position);

/* FLEX COUNTER FLEXIBLE POOL ALLOCATION AND
 * COMPACTION SUPPORT
 */
typedef struct compaction_status_s {
   uint32 pool_number;
   uint32 base_idx;
   int    in_progress;
}compaction_status_t;

extern
int _bcm_esw_stat_custom_group_id_create(int  unit,
        uint32                         mode_id,
        bcm_stat_object_t              object,
        uint32                         pool_id,
        uint32                         base_idx,
        bcm_stat_custom_counter_info_t *counter_info);

extern
int _bcm_stat_flex_reserve_base_index(int unit,
        bcm_stat_flex_direction_t direction, 
        uint32 pool_number_l, uint32 total_counters,
        uint32 base_idx, uint32 used_by_table,
        uint32 pipe_num, bcm_stat_object_t object);

extern
int _bcm_stat_flex_counter_id_move (int   unit,
        bcm_stat_custom_base_index_action_t idx_action,
        bcm_stat_custom_counter_info_t   counter_info_old,
        bcm_stat_custom_counter_info_t   *counter_info_new);

extern
int _bcm_stat_flex_counter_base_idx_move(int unit,
        bcm_stat_custom_counter_info_t   counter_info_old,
        bcm_stat_custom_counter_info_t   *counter_info_new);
extern
int _bcm_stat_flex_dump_pool_info(int unit);

extern
int _bcm_esw_stat_validate_counter_info(int unit,
        bcm_stat_custom_counter_info_t   counter_info_old,
        bcm_stat_custom_counter_info_t   *counter_info_new);
#if defined(BCM_TOMAHAWK_SUPPORT)
/* Accessor functions to set/get ifp color map database */
extern bcm_error_t _bcm_esw_ifp_color_map_set(
        int                   unit,
        uint32                stat_counter_id,
        _bcm_ifp_color_map_t  *color_map);
extern bcm_error_t _bcm_esw_ifp_color_map_get(
        int                   unit,
        uint32                stat_counter_id,
        _bcm_ifp_color_map_t  *color_map);
bcm_error_t _bcm_esw_get_fp_pipe_from_mode(int unit, int offset_mode, bcm_field_qualify_t stage, int *pipe);
int _bcm_esw_get_fp_mode_global(int unit, bcm_field_qualify_t stage);
bcm_field_qualify_t _bcm_esw_stat_get_field_stage_from_stat_ctr(int unit,
                                                 uint32 stat_counter_id);
bcm_error_t _bcm_esw_stat_flex_get_hw_mode(int unit, uint32 mode_id, uint32 *hw_mode_id);
bcm_error_t _bcm_esw_stat_flex_check_ingress_em_table_entry(
    int               unit,
    soc_mem_t         ingress_table,
    _field_stage_id_t field_stage,
    uint32            offset_mode,
    uint32            pool_number,
    uint32            base_idx);
void stat_ctr_map_init(int unit);
void stat_ctr_map_deinit(int unit);
int _bcm_esw_stat_pool_reserved(int unit,
                            bcm_stat_flex_direction_t direction, int pool);
int _bcm_esw_stat_mod_reserved(int unit, int mode_id);
extern bcm_error_t _bcm_stat_flex_flowtracker_counter_info_get(int unit,
                            bcm_stat_flex_flowtracker_counter_info_t *info);
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT) || defined(BCM_TRIDENT2PLUS_SUPPORT)
bcm_field_qualify_t _bcm_esw_stat_get_field_stage_from_table(soc_mem_t ingress_table); 
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_TOMAHAWK3_SUPPORT)
extern bcm_error_t _bcm_esw_stat_flex_get_custom_egress_mode_info(
            int                          unit,
            bcm_stat_flex_mode_t         mode,
            bcm_stat_flex_custom_egress_mode_t *flex_egress_mode);

extern bcm_error_t _bcm_esw_stat_flex_update_custom_egress_mode_info(
            int                          unit,
            bcm_stat_flex_mode_t         mode,
            bcm_stat_flex_custom_egress_mode_t *flex_egress_mode);
#endif

#endif

#if defined(BCM_KATANA2_SUPPORT)

typedef struct class_attr_combine_s {
    uint32 **class_pkt_attr_comb;
}class_attr_combine_t;

extern int _bcm_esw_stat_group_mode_attr_class_id_create(
               int unit,
               bcm_stat_group_mode_attr_t attr,
               int num_of_attr_values,
               uint32 *attr_values,
               bcm_stat_group_mode_attr_class_id_t *attr_class_id);

extern int _bcm_esw_stat_group_mode_attr_class_id_destroy(
                int unit,
                bcm_stat_group_mode_attr_class_id_t attr_class_id);

extern int _bcm_esw_stat_group_mode_attr_class_id_cleanup(int unit);

extern int _bcm_esw_stat_combine_attr_matrix_init(int unit,
               int *size);

extern int _bcm_esw_stat_flex_compressed_attr_matrix_sync(int unit,
               uint8 **scache_position);

extern int _bcm_esw_stat_flex_compressed_attr_matrix_recover(int unit,
               uint8 **scache_position);

#endif

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_APACHE_SUPPORT)
typedef struct stat_port_group_s {
    pbmp_t port_grp_id_bmap;
    pbmp_t *port_grp_bmap;
}stat_port_group_t;

extern int
_bcm_esw_stat_combine_port_bitmap_cleanup(int unit);

extern int
_bcm_esw_stat_combine_port_bitmap_init(int unit,
                                       int *size);

extern int
_bcm_esw_stat_port_group_create(int unit,
        uint32 num_gports,
        bcm_gport_t *gport,
        uint32 *group_id);

extern int
_bcm_esw_stat_port_group_destroy(int unit,
        uint32 group_id);

extern int
_bcm_esw_stat_combine_port_bitmap_sync(int unit,
        uint8 **scache_position);

extern int
_bcm_esw_stat_combine_port_bitmap_recover(int unit,
        uint8 **scache_position);

extern int
bcmi_flex_ctr_update_portmap(int unit, int mode, int valid, int dir);

extern int
bcmi_flex_ctr_cmpsd_port_sanitize(int unit, int mode, int dir,
        bcm_stat_flex_ing_cmprsd_attr_selectors_t *icmprsd_attr_selectors,
        bcm_stat_flex_egr_cmprsd_attr_selectors_t *ecmprsd_attr_selectors,
        int match);

extern int
_bcm_esw_stat_attr_subport_only(int unit,
                    bcm_stat_group_mode_attr_selector_t *attr_selectors,
                    int num_selectors);

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
/* FlexCounter Related */
typedef struct bcmi_flex_ctr_subport_info_s {
    int port_map_valid;
    int pool;
    uint8 portmap[FB6_TOTAL_NUM_SUBPORT];
} bcmi_flex_ctr_subport_info_t;

/* ING and EGR portmap for 4 modes */
extern bcmi_flex_ctr_subport_info_t ing_port_map_info[BCM_MAX_NUM_UNITS][BCM_STAT_FLEX_COUNTER_MAX_MODE];
extern bcmi_flex_ctr_subport_info_t egr_port_map_info[BCM_MAX_NUM_UNITS][BCM_STAT_FLEX_COUNTER_MAX_MODE];

extern void
bcmi_flex_ctr_portmap_info_init(int unit);
#endif

#endif
#endif /* __BCM_FLEX_CTR_H__ */
