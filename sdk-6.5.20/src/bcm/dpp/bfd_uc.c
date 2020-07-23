/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oam.c
 * Purpose: Manages L3 interface table, forwarding table, routing table
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_BFD

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/bfd.h>
#include <bcm_int/dpp/bfd_uc.h>
#include <bcm_int/dpp/bfd_feature.h>
#include <bcm_int/dpp/bfd_sdk_pack.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/bfd.h>
#include <shared/alloc.h>
#include <soc/uc_msg.h>
#include <soc/uc.h>
#include <soc/shared/bfd_msg.h>
#include <soc/shared/bfd.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>

/***************************************************************/
/***************************************************************/

#define BFDIPV6_ACCESS    sw_state_access[unit].dpp.bcm.bfd_ipv6

#define BCM_BFD_THREAD_PRI_DFLT     200

/*
 * Local defines for uKernel application
 */
#define _BCM_BFD_UDP_SRC_PORT_MIN (49152)
#define _BCM_BFD_UDP_SRC_PORT_MAX (65535)
#define _BCM_BFD_UDP_SRC_PORT_VALIDATE(a_)                                  \
    do { \
        if (((a_) < _BCM_BFD_UDP_SRC_PORT_MIN) || ((a_) > _BCM_BFD_UDP_SRC_PORT_MAX)) { \
            return BCM_E_PARAM;                                             \
        } \
    } while(0)

#define _BCM_BFD_RX_CHANNEL         1
#define _BCM_BFD_SDK_VERSION        0x01000001
#define _BCM_BFD_UC_MIN_VERSION     0x01000200
#define _BCM_BFD_COSQ_INVALID       0xffff

/*
 * Maximum number of BFD sessions
 * Session IDs valid range is  [0..2046]
 * Entry 0x7FF (2047) is reserved for unknown remote discriminator
 */
#define _BCM_BFD_MAX_NUM_SESSIONS  2047

/*
 * Maximum number of BFD endpoints
 * Endpoint IDs valid range is  [0..16382]
 * Entry 0x3FFF (16383) is reserved for unknown remote discriminator
 */
#define _BCM_BFD_MAX_NUM_ENDPOINTS  16383

/* Timeout for Host <--> uC message */
#define _BCM_BFD_UC_MSG_TIMEOUT_USECS   20000000 /* 20 secs */

/*
 * BFD Encapsulation Format Header flags
 *
 * Indicates the type of headers/labels present in a BFD packet.
 */
#define _BCM_BFD_ENCAP_PKT_MPLS                    (1 << 0) 
#define _BCM_BFD_ENCAP_PKT_MPLS_ROUTER_ALERT       (1 << 1) 
#define _BCM_BFD_ENCAP_PKT_PW                      (1 << 2) 
#define _BCM_BFD_ENCAP_PKT_GAL                     (1 << 3) 
#define _BCM_BFD_ENCAP_PKT_G_ACH                   (1 << 4) 
#define _BCM_BFD_ENCAP_PKT_GRE                     (1 << 5) 
#define _BCM_BFD_ENCAP_PKT_INNER_IP                (1 << 6)
#define _BCM_BFD_ENCAP_PKT_IP                      (1 << 7) 
#define _BCM_BFD_ENCAP_PKT_UDP                     (1 << 8) 
#define _BCM_BFD_ENCAP_PKT_BFD                     (1 << 9)
#define _BCM_BFD_ENCAP_PKT_HG                      (1 << 10)
#define _BCM_BFD_ENCAP_PKT_HG2                     (1 << 11)

#define _BCM_BFD_ENCAP_PKT_UDP__MULTI_HOP          (1 << 16) 
#define _BCM_BFD_ENCAP_PKT_INNER_IP__V6            (1 << 17)
#define _BCM_BFD_ENCAP_PKT_IP__V6                  (1 << 18) 
#define _BCM_BFD_ENCAP_PKT_G_ACH__IP               (1 << 19)
#define _BCM_BFD_ENCAP_PKT_G_ACH__CC               (1 << 20)
#define _BCM_BFD_ENCAP_PKT_G_ACH__CC_CV            (1 << 21)


/*
 * BFD Encapsulation Definitions
 *
 * Defines for building the BFD packet encapsulation
 */

#define _BCM_BFD_MAC_ADDR_LENGTH    (sizeof(bcm_mac_t))

#define _BCM_BFD_MPLS_MAX_LABELS    3   /* Max MPLS labels */

#define _BCM_BFD_DEFAULT_DETECT_MULTIPLIER   10 /* BFD Default Detection Time multiplier */   

/*
 * IPV4
 *
 * Loopback    127.0.0.0/8         127.0.0.0   - 127.255.255.255
 *
 * Reserved
 *   Class A   10.0.0.0/8          10.0.0.0    - 10.255.255.255
 *   Class B   172.16.0.0/12       172.16.0.0  - 172.31.255.255
 *   Class C   192.168.0.0/16      192.168.0.0 - 192.168.255.255  
 *   Class E   240.0.0.0
 */
#define IPV4_LOOPBACK(a_)    (127 == (((a_) >> 24) & 0xff))

#define IPV4_RESERVED_CLASS_A(a_)  ((((a_) >> 24) & 0xff) == 10)
#define IPV4_RESERVED_CLASS_B(a_)  (((((a_) >> 24) & 0xff) == 172) &&   \
                                    ((((a_) >> 16) & 0xf0) == 16))
#define IPV4_RESERVED_CLASS_C(a_)  (((((a_) >> 24) & 0xff) == 192) &&   \
                                    ((((a_) >> 16) & 0xff) == 168))
#define IPV4_RESERVED_CLASS_E(a_)  ((((a_) >> 24) & 0xff) == 240)

#define IPV4_RESERVED(a_)                                               \
    (IPV4_RESERVED_CLASS_A(a_) || IPV4_RESERVED_CLASS_B(a_) ||          \
     IPV4_RESERVED_CLASS_C(a_) || IPV4_RESERVED_CLASS_E(a_))

#define ENCAP_IPV4_LOOPBACK_VALIDATE(a_)            \
    if (!IPV4_LOOPBACK(a_)) { return BCM_E_PARAM; }

#define ENCAP_IPV4_ROUTABLE_VALIDATE(a_)            \
    if (IPV4_RESERVED(a_)) { return BCM_E_PARAM; } 

/*
 * IPV6
 * Loopback    0:0:0:0:0:FFFF:127/104
 *
 * Reserved
 *    fc00::/7
 */
#define IPV6_LOOPBACK(a_)                                                    \
    (((a_)[0] == 0) && ((a_)[1] == 0) && ((a_)[2] == 0) && ((a_)[3] == 0) && \
     ((a_)[4] == 0) && ((a_)[5] == 0) && ((a_)[6] == 0) && ((a_)[7] == 0) && \
     ((a_)[8] == 0) && ((a_)[9] == 0) &&                                     \
     ((a_)[10] == 0xff) && ((a_)[11] == 0xff) && ((a_)[12] == 127))
     

#define ENCAP_IPV6_LOOPBACK_VALIDATE(a_)            \
    if (!IPV6_LOOPBACK(a_)) { return BCM_E_PARAM; }

#define BFD_ENDPOINT_CONFIG(u_, index_, endpoint_config_)                   \
    {                                                                       \
        int _rv;                                                            \
        _rv = sw_state_access[u_].dpp.bcm.bfd_ipv6.endpoints.get(u_, index_, endpoint_config_);  \
        BCMDNX_IF_ERR_EXIT(_rv);                                            \
    }

/*
 * Utility macros
 */
/* True if BFD module has been initialized */

/* Checks for 'null' argument */
#define PARAM_NULL_CHECK(arg_)  \
    if ((arg_) == NULL) {       \
        return BCM_E_PARAM;     \
    }

/* Checks that required feature(s) is available */
#define FEATURE_CHECK(u_)                        \
    if (!soc_feature((u_), soc_feature_cmicm)) { \
        return BCM_E_UNAVAIL;                    \
    } 

/* Checks that BFD module has been initialized */
#define BFD_UC_INIT_CHECK(u_)                                           \
    if (!bcm_petra_bfd_uc_is_init(unit)) {                              \
        return BCM_E_INIT;                                              \
    }

/* Checks that required feature(s) is available and BFD has been initialized */
#define BFD_UC_FEATURE_INIT_CHECK(u_)               \
    do {                                            \
        FEATURE_CHECK(u_);                          \
        BFD_UC_INIT_CHECK(u_);                      \
    } while (0)


/* Checks that endpoint index is within valid range */
#define ENDPOINT_INDEX_CHECK(u_, index_)                                    \
    if ((index_) < 0 || (index_) >= BFD_IPV6_DPP_INFO(u_)->endpoint_count) {      \
        return BCM_E_PARAM;                                                 \
    }                                                                       

/* Checks that SHA1 Authentication index is valid */
#define AUTH_SHA1_INDEX_CHECK(u_, index_)                                   \
    if ((index_) >= BFD_IPV6_DPP_INFO(u_)->num_auth_sha1_keys) {                  \
        return BCM_E_PARAM;                                                 \
    }                                                                       

/* Checks that Simple Password Authentication index is valid */
#define AUTH_SP_INDEX_CHECK(u_, index_)                                 \
    if ((index_) >= BFD_IPV6_DPP_INFO(u_)->num_auth_sp_keys) {                \
        return BCM_E_PARAM;                                             \
    }                                                                   \

/* Moved out certain vaiables from bfd_info struct for which WB is not required */
static bfd_ipv6_no_wb_info_t * bcm_dpp_bfd_ipv6_info[SOC_MAX_NUM_DEVICES] = {0};

#define BFD_IPV6_DPP_INFO(u_)                     (bcm_dpp_bfd_ipv6_info[(u_)])


#define BFD_DPP_UC_INIT(u_)                                                 \
    ((BFD_IPV6_DPP_INFO(u_) != NULL) && (BFD_IPV6_DPP_INFO(u_)->initialized))

/* Size of the debug log buffer */
#define BFD_TRACE_LOG_MAX_SIZE 1024 /* 2KB */

/* UDP Header */
typedef struct bfd_udp_header_s {  /* <num bits>: ... */
    uint16  src;         /* 16: Source Port Number */
    uint16  dst;         /* 16: Destination Port Number */
    uint16  length;      /* 16: Length */
    uint16  sum;         /* 16: Checksum */
} bfd_udp_header_t;

/* IPv4 Header */
typedef struct bfd_ipv4_header_s {
    uint8   version;     /*  4: Version */
    uint8   h_length;    /*  4: Header length */
    uint8   dscp;        /*  6: Differentiated Services Code Point */
    uint8   ecn;         /*  2: Explicit Congestion Notification */
    uint16  length;      /* 16: Total Length bytes (header + data) */
    uint16  id;          /* 16: Identification */
    uint8   flags;       /*  3: Flags */
    uint16  f_offset;    /* 13: Fragment Offset */
    uint8   ttl;         /*  8: Time to Live */
    uint8   protocol;    /*  8: Protocol */
    uint16  sum;         /* 16: Header Checksum */
    uint32  src;         /* 32: Source IP Address */
    uint32  dst;         /* 32: Destination IP Address */
} bfd_ipv4_header_t;

/* IPv6 Header */
typedef struct bfd_ipv6_header_s {
    uint8      version;        /*   4: Version */
    uint8      t_class;        /*   8: Traffic Class (6:dscp, 2:ecn) */
    uint32     f_label;        /*  20: Flow Label */
    uint16     p_length;       /*  16: Payload Length */
    uint8      next_header;    /*   8: Next Header */
    uint8      hop_limit;      /*   8: Hop Limit */
    bcm_ip6_t  src;            /* 128: Source IP Address */
    bcm_ip6_t  dst;            /* 128: Destination IP Address */
} bfd_ipv6_header_t;

/* GRE - Generic Routing Encapsulation */
typedef struct bfd_gre_header_s {
    uint8   checksum;           /*  1: Checksum present */
    uint8   routing;            /*  1: Routing present */
    uint8   key;                /*  1: Key present */
    uint8   sequence_num;       /*  1: Sequence number present */
    uint8   strict_src_route;   /*  1: Strict Source Routing */
    uint8   recursion_control;  /*  3: Recursion Control */
    uint8   flags;              /*  5: Flags */
    uint8   version;            /*  3: Version */
    uint16  protocol;           /* 16: Protocol Type */
} bfd_gre_header_t;

/* ACH - Associated Channel Header */
typedef struct bfd_ach_header_s {
    uint8   f_nibble;        /*  4: First nibble, must be 1 */
    uint8   version;         /*  4: Version */
    uint8   reserved;        /*  8: Reserved */
    uint16  channel_type;    /* 16: Channel Type */
} bfd_ach_header_t;

/* MPLS - Multiprotocol Label Switching Label */
typedef struct bfd_mpls_label_s {
    uint32  label;    /* 20: Label */
    uint8   exp;      /*  3: Experimental, Traffic Class, ECN */
    uint8   s;        /*  1: Bottom of Stack */
    uint8   ttl;      /*  8: Time to Live */
} bfd_mpls_label_t;

/* VLAN Tag - 802.1Q */
typedef struct bfd_vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint8   prio;    /*  3: Priority Code Point */
        uint8   cfi;     /*  1: Canonical Format Indicator */
        uint16  vid;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} bfd_vlan_tag_t;

/* L2 Header */
typedef struct bfd_l2_header_t {
    bcm_mac_t       dst_mac;      /* 48: Destination MAC */
    bcm_mac_t       src_mac;      /* 48: Source MAC */
    bfd_vlan_tag_t  outer_vlan_tag;    /* VLAN Tag */
    bfd_vlan_tag_t  inner_vlan_tag;    /* inner VLAN Tag */
    uint16          etype;        /* 16: Ether Type */
} bfd_l2_header_t;

/* UDH Header */
typedef struct bfd_udh_header_s {
    uint8           size;     
    uint8           value[16];
} bfd_udh_header_t;


/* PTCH-2 Header */
typedef union bfd_ptch2_header_u {
    struct {
#if defined(LE_HOST)
        uint16          rsv;
        uint16          data;        
#else        
        uint16          data;
        uint16          rsv;
#endif
    } raw;

    struct {
#if defined(LE_HOST)
        uint32          rsv:16;
        uint32          pp_ssp:8;
        uint32          reserved:4;
        uint32          opaque_pt_attributes:3;
        uint32          parser_program_control:1;
#else        
        uint32          parser_program_control:1;
        uint32          opaque_pt_attributes:3;
        uint32          reserved:4;
        uint32          pp_ssp:8;
        uint32          rsv:16;
#endif
    } arad_common;
} bfd_ptch2_header_t;

/* Itmh Header */
typedef union bfd_itmh_header_u {
    struct {
        uint32          data;
    } raw;

    struct {
#if defined(LE_HOST)
        uint32          itmh_base_extension_exists:1;
        uint32          fwd_traffic_class:3;
        uint32          snoop_cmp:4;
        uint32          fwd_dest_info:19;
        uint32          fwd_dp:2;
        uint32          in_mirror_disable:1;
        uint32          pph_type:2;
#else
        uint32          pph_type:2;
        uint32          in_mirror_disable:1;
        uint32          fwd_dp:2;
        uint32          fwd_dest_info:19;
        uint32          snoop_cmp:4;
        uint32          fwd_traffic_class:3;
        uint32          itmh_base_extension_exists:1;
#endif
    } jericho_common;

    struct {
#if defined(LE_HOST)
        uint32          outlif:20;
        uint32          fwd_dp:2;
        uint32          fwd_traffic_class:3;
        uint32          snoop_cmp:4;
        uint32          in_mirror_flag:1;
        uint32          pph_type:2;
#else
        uint32          pph_type:2;
        uint32          in_mirror_flag:1;
        uint32          snoop_cmp:4;
        uint32          fwd_traffic_class:3;
        uint32          fwd_dp:2;
        uint32          outlif:20;
#endif
    } arad_common;
} bfd_itmh_header_t;

/* Itmh Ingress destination info extension Header */
typedef union bfd_itmh_ext_header_u {
    struct {
#if defined(LE_HOST)
        uint8           rsv;
        uint8           data[3];        
#else        
        uint8           data[3];
        uint8           rsv;
#endif
    } raw;

    struct {
#if defined(LE_HOST)
        uint32          rsv:8;
        uint32          fwd_dest_info:20;
        uint32          reserved:1;
        uint32          fwd_dest_info_type:3;   
#else
        uint32          fwd_dest_info_type:3;
        uint32          reserved:1;
        uint32          fwd_dest_info:20;
        uint32          rsv:8;
#endif
    } jericho_common;

    struct {
#if defined(LE_HOST)
        uint32          rsv:8;
        uint32          fwd_dest_info:16;
        uint32          fwd_dest_info_type:4;
        uint32          reserved:4;        
#else
        uint32          reserved:4;
        uint32          fwd_dest_info_type:4;
        uint32          fwd_dest_info:16;
        uint32          rsv:8;
#endif
    } arad_common;
} bfd_itmh_ext_header_t;

/* PPH Header */
typedef union bfd_pph_header_u {
    struct {
#if defined(LE_HOST)
        uint8           rsv;
        uint8           data[7];        
#else
        uint8           data[7];
        uint8           rsv;
#endif
    } raw;

    struct {
#if defined(LE_HOST)
        uint32          rsv:8;
        uint32          inlif_inrif:18;
        uint32          vsi_vrf_lo:6;
        uint32          vsi_vrf_hi:10;
        uint32          learn_allowed:1;
        uint32          unknown_da:1;
        uint32          inlif_orientiation:2;
        uint32          snoop_cpu_code:2;
        uint32          pkt_is_ctrl:1;
        uint32          fwd_header_offset:7;
        uint32          fwd_code:4;
        uint32          fhei_size:2;
        uint32          learn_ext_present:1;
        uint32          eei_ext_present:1;
#else
        uint32          eei_ext_present:1;
        uint32          learn_ext_present:1;
        uint32          fhei_size:2;
        uint32          fwd_code:4;
        uint32          fwd_header_offset:7;
        uint32          pkt_is_ctrl:1;
        uint32          snoop_cpu_code:2;
        uint32          inlif_orientiation:2;
        uint32          unknown_da:1;
        uint32          learn_allowed:1;
        uint32          vsi_vrf_hi:10;
        uint32          vsi_vrf_lo:6;
        uint32          inlif_inrif:18;
        uint32          rsv:8;
#endif
    } arad_common;
} bfd_pph_header_t;


/*
 * Function:
 *      _bcm_petra_bfd_uc_alloc_clear
 * Purpose:
 *      Allocate memory block and set memory content to zeros.
 * Parameters:
 *      size        - (IN) Size of memory to allocate.
 *      description - (IN) Description of memory block.
 * Returns:
 *      Pointer to memory block.
 */
STATIC void *
_bcm_petra_bfd_uc_alloc_clear(unsigned int size, char *description)
{
    void *block = NULL;

    if (size) {
        block = sal_alloc(size, description);
        if (block != NULL) {
            sal_memset(block, 0, size);
        }
    }

    return block;
}


/*
 * Function:
 *      _bcm_petra_bfd_uc_free_resource
 * Purpose:
 *      Free memory resources allocated for given unit.
 * Parameters:
 *      unit       - (IN) Unit number.
 * Returns:
 *      None
 */
STATIC void
_bcm_petra_bfd_uc_free_resource(int unit)
{
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    if (bcm_dpp_bfd_ipv6_info[unit] != NULL) {
        if (bfd_ipv6_dpp_info->dma_buffer != NULL) {
            soc_cm_sfree(unit, bfd_ipv6_dpp_info->dma_buffer);
        }

        sal_free(bcm_dpp_bfd_ipv6_info[unit]);
        bcm_dpp_bfd_ipv6_info[unit] = NULL;
    }
    return ;
}


void
bcm_petra_memcpy_to_bfd_endpoint_info(bfd_endpoint_info_t *dst_info, bcm_bfd_endpoint_info_t *src_info)
{
    dst_info->flags = src_info->flags;
    dst_info->id = src_info->id;
    dst_info->remote_id = src_info->remote_id;
    dst_info->type = src_info->type;
    dst_info->gport = src_info->gport;
    dst_info->tx_gport = src_info->tx_gport;
    dst_info->remote_gport = src_info->remote_gport;
    dst_info->bfd_period = src_info->bfd_period;
    dst_info->vpn = src_info->vpn;
    dst_info->vlan_pri = src_info->vlan_pri;
    dst_info->inner_vlan_pri = src_info->inner_vlan_pri;
    dst_info->vrf_id = src_info->vrf_id;
    dst_info->dst_ip_addr = src_info->dst_ip_addr;
    dst_info->src_ip_addr = src_info->src_ip_addr;
    sal_memcpy(dst_info->src_ip6_addr,src_info->src_ip6_addr,
        BCM_IP6_ADDRLEN);
    sal_memcpy(dst_info->dst_ip6_addr, src_info->dst_ip6_addr,
        BCM_IP6_ADDRLEN);
    dst_info->ip_tos = src_info->ip_tos;
    dst_info->ip_ttl = src_info->ip_ttl;
    dst_info->inner_dst_ip_addr = src_info->inner_dst_ip_addr;
    sal_memcpy(dst_info->inner_dst_ip6_addr,src_info->inner_dst_ip6_addr,
        BCM_IP6_ADDRLEN);
    dst_info->inner_src_ip_addr = src_info->inner_src_ip_addr;
    sal_memcpy(dst_info->inner_src_ip6_addr,src_info->inner_src_ip6_addr,
        BCM_IP6_ADDRLEN);
    dst_info->inner_ip_tos = src_info->inner_ip_tos;
    dst_info->inner_ip_ttl = src_info->inner_ip_ttl;
    dst_info->udp_src_port = src_info->udp_src_port;
    dst_info->label = src_info->label;
    sal_memcpy(&dst_info->egress_label,&src_info->egress_label,
        sizeof(bcm_mpls_egress_label_t));
    dst_info->egress_if = src_info->egress_if;
    sal_memcpy(dst_info->mep_id,src_info->mep_id,BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH);
    dst_info->mep_id_length = src_info->mep_id_length;
    dst_info->int_pri = src_info->int_pri;
    dst_info->cpu_qid = src_info->cpu_qid;
    dst_info->local_state = src_info->local_state;
    dst_info->local_discr = src_info->local_discr;
    dst_info->local_diag = src_info->local_diag;
    dst_info->local_flags = src_info->local_flags;
    dst_info->local_min_tx = src_info->local_min_tx;
    dst_info->local_min_rx = src_info->local_min_rx;
    dst_info->local_min_echo = src_info->local_min_echo;
    dst_info->local_detect_mult = src_info->local_detect_mult;
    sal_memcpy(&dst_info->auth,&src_info->auth,sizeof(bcm_bfd_auth_type_t));
    dst_info->auth_index = src_info->auth_index;
    dst_info->tx_auth_seq = src_info->tx_auth_seq;
    dst_info->rx_auth_seq = src_info->rx_auth_seq;
    dst_info->remote_flags = src_info->remote_flags;
    dst_info->remote_state = src_info->remote_state;
    dst_info->remote_discr = src_info->remote_discr;
    dst_info->remote_diag = src_info->remote_diag;
    dst_info->remote_min_tx = src_info->remote_min_tx;
    dst_info->remote_min_rx = src_info->remote_min_rx;
    dst_info->remote_min_echo = src_info->remote_min_echo;
    dst_info->remote_detect_mult = src_info->remote_detect_mult;
    dst_info->sampling_ratio = src_info->sampling_ratio;
    dst_info->loc_clear_threshold = src_info->loc_clear_threshold;
    dst_info->ip_subnet_length = src_info->ip_subnet_length;
    sal_memcpy(dst_info->remote_mep_id,src_info->remote_mep_id,BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH);
    dst_info->remote_mep_id_length = src_info->remote_mep_id_length;
    sal_memcpy(dst_info->mis_conn_mep_id,src_info->mis_conn_mep_id,BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH);
    dst_info->mis_conn_mep_id_length = src_info->mis_conn_mep_id_length;
    dst_info->bfd_detection_time = src_info->bfd_detection_time;
    dst_info->pkt_vlan_id = src_info->pkt_vlan_id;
    dst_info->rx_pkt_vlan_id = src_info->rx_pkt_vlan_id;
    dst_info->bfd_detection_time = src_info->bfd_detection_time;
    dst_info->bfd_period = src_info->bfd_period;
}

void
bcm_petra_memcpy_to_bcm_bfd_endpoint_info(bcm_bfd_endpoint_info_t *dst_info, bfd_endpoint_info_t *src_info)
{
    dst_info->flags = src_info->flags;
    dst_info->id = src_info->id;
    dst_info->remote_id = src_info->remote_id;
    dst_info->type = src_info->type;
    dst_info->gport = src_info->gport;
    dst_info->tx_gport = src_info->tx_gport;
    dst_info->remote_gport = src_info->remote_gport;
    dst_info->bfd_period = src_info->bfd_period;
    dst_info->vpn = src_info->vpn;
    dst_info->vlan_pri = src_info->vlan_pri;
    dst_info->inner_vlan_pri = src_info->inner_vlan_pri;
    dst_info->vrf_id = src_info->vrf_id;
    dst_info->dst_ip_addr = src_info->dst_ip_addr;
    dst_info->src_ip_addr = src_info->src_ip_addr;
    sal_memcpy(dst_info->src_ip6_addr,src_info->src_ip6_addr,
        BCM_IP6_ADDRLEN);
    sal_memcpy(dst_info->dst_ip6_addr, src_info->dst_ip6_addr,
        BCM_IP6_ADDRLEN);
    dst_info->ip_tos = src_info->ip_tos;
    dst_info->ip_ttl = src_info->ip_ttl;
    dst_info->inner_dst_ip_addr = src_info->inner_dst_ip_addr;
    sal_memcpy(dst_info->inner_dst_ip6_addr,src_info->inner_dst_ip6_addr,
        BCM_IP6_ADDRLEN);
    dst_info->inner_src_ip_addr = src_info->inner_src_ip_addr;
    sal_memcpy(dst_info->inner_src_ip6_addr,src_info->inner_src_ip6_addr,
        BCM_IP6_ADDRLEN);
    dst_info->inner_ip_tos = src_info->inner_ip_tos;
    dst_info->inner_ip_ttl = src_info->inner_ip_ttl;
    dst_info->udp_src_port = src_info->udp_src_port;
    dst_info->label = src_info->label;
    sal_memcpy(&dst_info->egress_label,&src_info->egress_label,
        sizeof(bcm_mpls_egress_label_t));
    dst_info->egress_if = src_info->egress_if;
    sal_memcpy(dst_info->mep_id,src_info->mep_id,BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH);
    dst_info->mep_id_length = src_info->mep_id_length;
    dst_info->int_pri = src_info->int_pri;
    dst_info->cpu_qid = src_info->cpu_qid;
    dst_info->local_state = src_info->local_state;
    dst_info->local_discr = src_info->local_discr;
    dst_info->local_diag = src_info->local_diag;
    dst_info->local_flags = src_info->local_flags;
    dst_info->local_min_tx = src_info->local_min_tx;
    dst_info->local_min_rx = src_info->local_min_rx;
    dst_info->local_min_echo = src_info->local_min_echo;
    dst_info->local_detect_mult = src_info->local_detect_mult;
    sal_memcpy(&dst_info->auth,&src_info->auth,sizeof(bcm_bfd_auth_type_t));
    dst_info->auth_index = src_info->auth_index;
    dst_info->tx_auth_seq = src_info->tx_auth_seq;
    dst_info->rx_auth_seq = src_info->rx_auth_seq;
    dst_info->remote_flags = src_info->remote_flags;
    dst_info->remote_state = src_info->remote_state;
    dst_info->remote_discr = src_info->remote_discr;
    dst_info->remote_diag = src_info->remote_diag;
    dst_info->remote_min_tx = src_info->remote_min_tx;
    dst_info->remote_min_rx = src_info->remote_min_rx;
    dst_info->remote_min_echo = src_info->remote_min_echo;
    dst_info->remote_detect_mult = src_info->remote_detect_mult;
    dst_info->sampling_ratio = src_info->sampling_ratio;
    dst_info->loc_clear_threshold = src_info->loc_clear_threshold;
    dst_info->ip_subnet_length = src_info->ip_subnet_length;
    sal_memcpy(dst_info->remote_mep_id,src_info->remote_mep_id,BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH);
    dst_info->remote_mep_id_length = src_info->remote_mep_id_length;
    sal_memcpy(dst_info->mis_conn_mep_id,src_info->mis_conn_mep_id,BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH);
    dst_info->mis_conn_mep_id_length = src_info->mis_conn_mep_id_length;
    dst_info->bfd_detection_time = src_info->bfd_detection_time;
    dst_info->pkt_vlan_id = src_info->pkt_vlan_id;
    dst_info->rx_pkt_vlan_id = src_info->rx_pkt_vlan_id;
}


STATIC int
_bcm_petra_bfd_uc_alloc_resource(int unit, int num_sessions,
                             int num_auth_sha1_keys, int num_auth_sp_keys,
                              uint8 use_localdiscr_as_sess_id)
{
    int rv, num_auth;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info;
    uint32 max_endpoints = 0, session_map_size = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (bcm_dpp_bfd_ipv6_info[unit] != NULL) {
        _bcm_petra_bfd_uc_free_resource(unit);
    }

    /* Allocate memory for BFD Information Data Structure */
    if ((bcm_dpp_bfd_ipv6_info[unit] =
            _bcm_petra_bfd_uc_alloc_clear(sizeof(bfd_ipv6_no_wb_info_t),
                                    "BFD IPV6 info")) == NULL) {
        return BCM_E_MEMORY;
    }

    bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    /*
     * Allocate DMA buffer
     *
     * DMA buffer will be used to send and receive 'long' messages
     * between SDK Host and uController (BTE).
     */

    bfd_ipv6_dpp_info->dma_buffer_len = sizeof(bfd_sdk_msg_ctrl_t);
    bfd_ipv6_dpp_info->dma_buffer = soc_cm_salloc(unit, bfd_ipv6_dpp_info->dma_buffer_len,
                                                "BFD DMA buffer");
    if (!bfd_ipv6_dpp_info->dma_buffer) {
        return BCM_E_MEMORY;
    }
    sal_memset(bfd_ipv6_dpp_info->dma_buffer, 0, bfd_ipv6_dpp_info->dma_buffer_len);


#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
#endif

    rv = BFDIPV6_ACCESS.endpoints.alloc(unit, num_sessions);
    BCMDNX_IF_ERR_EXIT(rv) ;


    bfd_ipv6_dpp_info->use_local_discr_as_session_id = use_localdiscr_as_sess_id;


    if (use_localdiscr_as_sess_id) {
        max_endpoints = _BCM_BFD_MAX_NUM_ENDPOINTS;
    } else {
        max_endpoints = num_sessions;
    }
    rv = BFDIPV6_ACCESS.endpoints_in_use.alloc_bitmap(unit, max_endpoints);
    BCMDNX_IF_ERR_EXIT(rv) ;

    if (use_localdiscr_as_sess_id) {
        rv = BFDIPV6_ACCESS.sessions_in_use.alloc_bitmap(unit, num_sessions);
        BCMDNX_IF_ERR_EXIT(rv) ;

        session_map_size = (num_sessions * sizeof(bcm_bfd_endpoint_t));
        rv = BFDIPV6_ACCESS.endpoint_to_session_map.alloc(unit,
                                                          session_map_size);
        BCMDNX_IF_ERR_EXIT(rv) ;
    }

    /* Allocate memory for BFD Authentications */
    if (num_auth_sha1_keys > 0) {
        num_auth = num_auth_sha1_keys * sizeof(bcm_bfd_auth_sha1_t);
        rv = BFDIPV6_ACCESS.auth_sha1.alloc(unit, num_auth);
        BCMDNX_IF_ERR_EXIT(rv) ;
    }
    if (num_auth_sp_keys > 0) {
        num_auth = num_auth_sp_keys * sizeof(bcm_bfd_auth_simple_password_t);
        rv = BFDIPV6_ACCESS.auth_sp.alloc(unit, num_auth);
        BCMDNX_IF_ERR_EXIT(rv) ;
    }
#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif

    exit:
        BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *      _bcm_petra_bfd_uc_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_petra_bfd_uc_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_BFD_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_BFD_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_BFD_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_BFD_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_BFD_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_BFD_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_BFD_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_BFD_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_BFD_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_BFD_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_BFD_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_BFD_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_BFD_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_BFD_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_BFD_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_BFD_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_BFD_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_BFD_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_BFD_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

/**
 * RX: Set trap code for remote gport.
 * 
 * @author lingyong (10/11/2015)
 * 
 * @param unit 
 * @param endpoint_info 
 * @param trap_code 
 * 
 * @return int 
 */
STATIC int _bcm_petra_bfd_uc_trap_code_create(int unit, bcm_bfd_endpoint_info_t *endpoint_info, uint32 *trap_code) {
    int rv;    
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
    classifier_mep_entry.flags |= ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) != 0) ? SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED : 0;
 
    if (BCM_GPORT_IS_TRAP(endpoint_info->remote_gport)) {
        /* Use destination set to a pre defined trap code*/
        rv = _bcm_dpp_oam_bfd_trap_code_get_and_prepare_for_action(unit, &classifier_mep_entry, 0 /* not relevant */, 0 /* not relevant */,
                                                                   endpoint_info->remote_gport, BCM_GPORT_INVALID, trap_code, trap_code);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: The remote_gport is not support.\n")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/**
 * RX: Set OAM classifier.
 * 
 * @author sinai (17/11/2014)
 * 
 * @param unit 
 * @param endpoint_info 
 * @param endpoint_config 
 * 
 * @return int 
 */
STATIC int _bcm_petra_bfd_uc_m_hop_set_classifer(int unit, bcm_bfd_endpoint_info_t *endpoint_info, uint32 trap_code, uint8 trap_strength, bfd_endpoint_config_t *endpoint_config) {
    int rv;
    uint32 soc_sand_rv;
    SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    int cur_range_min, cur_range_max,discr_msbs;

    int is_allocated, profile, dip_ndx;
    uint8 internal_bfd_opcode;

    BCMDNX_INIT_FUNC_DEFS;


    if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
        /* First, set the IP DIP table. Incoming packet DIP should match endpoint's SIP.*/
        soc_sand_SAND_PP_IPV6_ADDRESS_clear(&ipv6_addr);
        rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, endpoint_info->src_ip6_addr, &ipv6_addr);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = _bcm_dpp_am_template_bfd_ip_dip_alloc(unit, 0 /*flags*/, &ipv6_addr, &is_allocated, &dip_ndx);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_allocated) {
            soc_sand_rv = soc_ppd_oam_bfd_my_bfd_dip_ip_set(unit, dip_ndx, &ipv6_addr);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        endpoint_config->dip_index = dip_ndx;
    }

    /* Now the actual classifer*/
    SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);

    /* get the internal bfd opcode */
    /* we use CCM opcode to signal BFD packets. This is not used in HW and isn't influence by opcode to internal opcode mapping*/
    rv = sw_state_access[unit].dpp.bcm.oam.y1731_opcode_to_intenal_opcode_map.get(unit,
        SOC_PPC_OAM_ETHERNET_PDU_OPCODE_CCM,
        &internal_bfd_opcode);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set the trap code: Should be either the CMIC destination or CPU.*/
    if (!trap_code) {
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) {
            rv = sw_state_access[unit].dpp.bcm.bfd.trap_info.trap_ids.get(unit, SOC_PPC_BFD_TRAP_ID_UC_IPV6, &trap_code); 
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            trap_code = SOC_PPC_TRAP_CODE_OAM_CPU;
        }
        /* For default trap codes, set default trap strength */
        trap_strength = _ARAD_PP_OAM_TRAP_STRENGTH;
    }
    profile_data.mep_profile_data.opcode_to_trap_code_multicast_map[internal_bfd_opcode] = trap_code;
    profile_data.mep_profile_data.opcode_to_trap_strength_multicast_map[internal_bfd_opcode] = trap_strength?trap_strength:_ARAD_PP_OAM_TRAP_STRENGTH;

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) {
        rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_alloc(unit, 0 /*flags*/, &profile_data, &is_allocated, &profile);
        BCMDNX_IF_ERR_EXIT(rv);
        classifier_mep_entry.acc_profile = profile;
        classifier_mep_entry.flags = SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT | SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED;
        endpoint_config->acc_mep_profile = profile;
    } else {
        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_alloc(unit, 0 /*flags*/, &profile_data, &is_allocated, &profile);
        BCMDNX_IF_ERR_EXIT(rv);

        classifier_mep_entry.flags |= SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT;
        classifier_mep_entry.non_acc_profile = profile;
        endpoint_config->non_acc_profile = profile;
    }
    classifier_mep_entry.mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV6_M_HOP;  /* As far as the classifier is concerned this may as welll be an IPv4 entry.*/
    classifier_mep_entry.your_discriminator = endpoint_info->local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_LIF_START_MASK;
    classifier_mep_entry.lif = _BCM_OAM_INVALID_LIF;

    /* Next, set the range*/
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oam_bfd_discriminator_rx_range_get, (unit,&cur_range_min, &cur_range_max ));
    BCMDNX_IF_ERR_EXIT(rv); 
    discr_msbs =  endpoint_info->local_discr >> SOC_PPC_BFD_RX_YOUR_DISCRIMINATOR_RANGE_BIT_START;
  
    rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_oam_oam_bfd_discriminator_rx_range_set, (unit, (discr_msbs <cur_range_min)? discr_msbs:cur_range_min ,
                                                                                      (discr_msbs >cur_range_max)? discr_msbs: cur_range_max));
    BCMDNX_IF_ERR_EXIT(rv); 

	/* Finally, set the classifier */
    soc_sand_rv = soc_ppd_oam_classifier_oem_mep_add(unit, endpoint_info->id, &classifier_mep_entry, 0/*update*/, 1);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (is_allocated) {
        if(endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) {
            soc_sand_rv = arad_pp_oam_classifier_oam1_entries_insert_bfd_acccelerated_to_profile(unit, &classifier_mep_entry, &profile_data);
        } else {
            soc_sand_rv = arad_pp_oam_classifier_oam1_entries_insert_bfd_according_to_profile(unit, &classifier_mep_entry, &profile_data);
        }
    }
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/**
 * 
 * 
 * @author sinai (19/11/2014)
 * 
 * @param unit 
 * @param endpoint_config 
 * 
 * @return int 
 */
STATIC int _bcm_petra_bfd_uc_m_hop_remove_classifer_entry(int unit, bfd_endpoint_config_t *endpoint_config) {
    int rv;
    uint32 soc_sand_rv;
    SOC_PPC_OAM_LIF_PROFILE_DATA profile_data;
    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY classifier_mep_entry;
    int is_last;

    BCMDNX_INIT_FUNC_DEFS;

    
    SOC_PPC_OAM_LIF_PROFILE_DATA_clear(&profile_data);

    if (endpoint_config->info.flags & BCM_BFD_ENDPOINT_IN_HW) {
        rv = _bcm_dpp_am_template_bfd_mep_profile_accelerated_free(unit, endpoint_config->acc_mep_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        rv = _bcm_dpp_am_template_bfd_mep_profile_non_accelerated_free(unit, endpoint_config->non_acc_profile, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_clear(&classifier_mep_entry);
    classifier_mep_entry.flags = SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ENDPOINT | SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY_FLAG_ACCELERATED;
    classifier_mep_entry.your_discriminator = endpoint_config->info.local_discr & SOC_PPC_BFD_DISCRIMINATOR_TO_LIF_START_MASK;
    classifier_mep_entry.lif = _BCM_OAM_INVALID_LIF;
    classifier_mep_entry.mep_type = SOC_PPC_OAM_MEP_TYPE_BFD_O_IPV6_M_HOP;  /* As far as the classifier is concerned this may as welll be an IPv4 entry.*/


    soc_sand_rv = soc_ppd_oam_classifier_mep_delete(unit, 0 /*MEP index: not really used */, &classifier_mep_entry);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (endpoint_config->info.flags & BCM_BFD_ENDPOINT_MULTIHOP) {
        rv = _bcm_dpp_am_template_bfd_ip_dip_free(unit, endpoint_config->dip_index, &is_last);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_last) {
            SOC_SAND_PP_IPV6_ADDRESS ipv6_addr;
            soc_sand_SAND_PP_IPV6_ADDRESS_clear(&ipv6_addr);

            soc_sand_rv = soc_ppd_oam_bfd_my_bfd_dip_ip_set(unit, endpoint_config->dip_index, &ipv6_addr);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      shr_llm_msg_send_receive
 * Purpose:
 *      Send a BFD message to ARM CPU.
 * Parameters:
 *      unit  (IN) unit number
 *      s_subclass (IN) send subclass
 *      s_len (IN) send length
 *      s_data (IN) send data
 *      r_len (OUT) receive length
 */
int _bcm_petra_bfd_uc_msg_send_receive(int unit, uint8 s_subclass, uint16 s_len, uint32 s_data, 
                             uint8 r_subclass, uint16 *r_len)
{
    int rv = SOC_E_NONE;
    mos_msg_data_t send, reply;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);
    uint8 *dma_buffer = bfd_ipv6_dpp_info->dma_buffer;
    int dma_buffer_len = bfd_ipv6_dpp_info->dma_buffer_len;
    int uc_num = bfd_ipv6_dpp_info->uc_num;

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = MOS_MSG_CLASS_BFD;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /* Set 'data' to DMA buffer address if a DMA operation is required for send or receive. */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /*LLM_MSG_LOCK(unit);*/
    rv = soc_cmic_uc_msg_send_receive(unit, uc_num,
                                      &send, &reply,
                                      _BCM_BFD_UC_MSG_TIMEOUT_USECS);
    /*LLM_MSG_UNLOCK(unit);*/

    /* Check reply class, subclass */
    if ((rv != SOC_E_NONE) ||
        (reply.s.mclass != MOS_MSG_CLASS_BFD) ||
        (reply.s.subclass != r_subclass)) {
        return SOC_E_INTERNAL;
    }

    *r_len = bcm_ntohs(reply.s.len);
    /* Convert BHH uController error code to BCM because ARM store the return code at r_data */
    rv = _bcm_petra_bfd_uc_convert_uc_error(bcm_ntohl(reply.s.data));

    return rv;
}

/*
 * Function:
 *      _bcm_petra_bfd_event_mask_set
 * Purpose:
 *      Set the BFD Events mask.
 *      Events are set per BFD module.
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_XXX  Operation failed
 * Notes:
 */
STATIC int
_bcm_petra_bfd_uc_event_mask_set(int unit)
{

    bfd_event_handler_t *event_handler;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);
    uint32 event_mask = 0;
    uint16 reply_len;

    /* Get event mask from all callbacks */
    for (event_handler = bfd_ipv6_dpp_info->event_handler_list;
         event_handler != NULL;
         event_handler = event_handler->next) {

        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventStateChange)) {
            event_mask |= BFD_BTE_EVENT_STATE;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventRemoteStateDiag)) {
            event_mask |= BFD_BTE_EVENT_REMOTE_STATE_DIAG;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventSessionDiscriminatorChange)) {
            event_mask |= BFD_BTE_EVENT_DISCRIMINATOR;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventAuthenticationChange)) {
            event_mask |= BFD_BTE_EVENT_AUTHENTICATION;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventParameterChange)) {
            event_mask |= BFD_BTE_EVENT_PARAMETER;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventSessionError)) {
            event_mask |= BFD_BTE_EVENT_ERROR;
        }        
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventEndpointFlagsChange)) {
            event_mask |= BFD_BTE_EVENT_FLAGS_CHANGE;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventEndpointTimeout)) {
            event_mask |= BFD_BTE_EVENT_SESSION_TIMEOUT;
        }
        if (SHR_BITGET(event_handler->event_types.w,
                       bcmBFDEventEndpointTimein)) {
            event_mask |= BFD_BTE_EVENT_SESSION_TIMEIN;
        }
    }

    /* Update BFD event mask in uKernel */
    if (event_mask != bfd_ipv6_dpp_info->event_mask) {
        /* Send BFD Event Mask message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_msg_send_receive
             (unit,
              MOS_MSG_SUBCLASS_BFD_EVENT_MASK_SET,
              0, event_mask,
              MOS_MSG_SUBCLASS_BFD_EVENT_MASK_SET_REPLY,
              &reply_len));

        if (reply_len != 0) {
            return BCM_E_INTERNAL;
        }
    }

    bfd_ipv6_dpp_info->event_mask = event_mask;

    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_petra_bfd_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to BFD info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_petra_bfd_uc_callback_thread(int unit)
{
    int rv;
    bcm_bfd_event_types_t events, cb_events;
    bfd_msg_event_t event_msg;
    bcm_bfd_endpoint_t sess_id;
    uint32 event_mask;
    int invoke = 0;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    int  uc_num = 0;
    bfd_event_handler_t *event_handler;
    uint8 use_localdiscr_as_sess_id = 0;
    bcm_bfd_endpoint_t endpoint_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "BFD callback thread starting\n")));
    thread_name[0] = 0x0;

    use_localdiscr_as_sess_id   =
                             bfd_ipv6_dpp_info->use_local_discr_as_session_id;

    bfd_ipv6_dpp_info->event_thread_id = sal_thread_self();
    bfd_ipv6_dpp_info->event_thread_kill = 0;
    sal_thread_name(bfd_ipv6_dpp_info->event_thread_id, thread_name, sizeof (thread_name));
    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(unit, uc_num,
                                     MOS_MSG_CLASS_BFD_EVENT, &event_msg,
                                     sal_sem_FOREVER);

        if (BCM_FAILURE(rv) || (bfd_ipv6_dpp_info->event_thread_kill)) {
            break;   /* Thread exit */
        }

        /* Get data from event message */
        sess_id    = bcm_ntohs(event_msg.s.len);
        event_mask = bcm_ntohl(event_msg.s.data);

        /* Set events */
        sal_memset(&events, 0, sizeof(events));
        if (event_mask & BFD_BTE_EVENT_STATE) {
            SHR_BITSET(events.w, bcmBFDEventStateChange);
        }
        if (event_mask & BFD_BTE_EVENT_REMOTE_STATE_DIAG) {
            SHR_BITSET(events.w, bcmBFDEventRemoteStateDiag);
        }
        if (event_mask & BFD_BTE_EVENT_DISCRIMINATOR) {
            SHR_BITSET(events.w, bcmBFDEventSessionDiscriminatorChange);
        }
        if (event_mask & BFD_BTE_EVENT_AUTHENTICATION) {
            SHR_BITSET(events.w, bcmBFDEventAuthenticationChange);
        }
        if (event_mask & BFD_BTE_EVENT_PARAMETER) {
            SHR_BITSET(events.w, bcmBFDEventParameterChange);
        }
        if (event_mask & BFD_BTE_EVENT_ERROR) {
            SHR_BITSET(events.w, bcmBFDEventSessionError);
        }
        if (event_mask & BFD_BTE_EVENT_FLAGS_CHANGE) {
            SHR_BITSET(events.w, bcmBFDEventEndpointFlagsChange);
        }
        if (event_mask & BFD_BTE_EVENT_SESSION_TIMEOUT) {
            SHR_BITSET(events.w, bcmBFDEventEndpointTimeout);
        }
        if (event_mask & BFD_BTE_EVENT_SESSION_TIMEIN) {
            SHR_BITSET(events.w, bcmBFDEventEndpointTimein);
        }

        if(use_localdiscr_as_sess_id) {
            rv = BFDIPV6_ACCESS.endpoint_to_session_map.get(unit,
                    sess_id,
                    &endpoint_id);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_BFD,
                        (BSL_META_U(unit,
                                    "failure in retriving EP ID"
                                    " for session ID:%d\n"), sess_id));
                continue;
            }
            _BCM_BFD_UC_MEP_INDEX_FROM_INTERNAL(endpoint_id, endpoint_id);
        } else {
            endpoint_id = sess_id;
            _BCM_BFD_UC_MEP_INDEX_FROM_INTERNAL(endpoint_id, endpoint_id);
        }
        /* Loop over registered callbacks,
         * If any match the events field, then invoke
         */
        sal_memset(&cb_events, 0, sizeof(cb_events));
        for (event_handler = bfd_ipv6_dpp_info->event_handler_list;
             event_handler != NULL;
             event_handler = event_handler->next) {
            SHR_BITAND_RANGE(event_handler->event_types.w, events.w,
                             0, bcmBFDEventCount, cb_events.w);
            SHR_BITTEST_RANGE(cb_events.w, 0, bcmBFDEventCount, invoke);

            if (invoke) {
                event_handler->cb(unit, 0,
                        cb_events, endpoint_id,
                        event_handler->user_data);
            }
        }
    }
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_BFD,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:%s\n"), thread_name)); 
    }

    bfd_ipv6_dpp_info->event_thread_kill = 0;
    bfd_ipv6_dpp_info->event_thread_id = NULL;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "BFD callback thread stopped\n")));
    sal_thread_exit(0);

}

/*
 * Function:
 *      _bcm_petra_bfd_uc_find_free_endpoint
 * Purpose:
 *      Find an available endpoint index (endpoint id).
 * Parameters:
 *      endpoints_in_use - Current endpoint ids usage bitmap.
 *      endpoint_count   - Max number of endpoints.
 * Returns:
 *      Available endpoint id.
 *      (-1) Indicates no more available endpoint ids.
 * Notes:
 */
STATIC int
_bcm_petra_bfd_uc_find_free_endpoint(int unit, int endpoint_count)
{
    int endpoint_index;
    uint8 ep_in_use;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    for (endpoint_index = 0; endpoint_index < endpoint_count;
         endpoint_index += 1) {
        rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, endpoint_index, &ep_in_use);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!ep_in_use) {
            break;
        }
    }

    if (endpoint_index >= endpoint_count) {
        endpoint_index = -1;
    }

    return endpoint_index;

    exit:
        BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_find_free_session
 * Purpose:
 *      Find an available session id.
 * Parameters:
 *      session_count   - Max number of sessions.
 * Returns:
 *      Available endpoint id.
 *      (-1) Indicates no more available session ids.
 * Notes:
 */
STATIC int
_bcm_petra_bfd_uc_find_free_session(int unit, int session_count)
{
    int session_index;
    uint8 session_in_use = 0;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    for (session_index = 0; session_index < session_count;
         session_index += 1) {
        rv = BFDIPV6_ACCESS.sessions_in_use.bit_get(unit,
                                                    session_index,
                                                    &session_in_use);
        BCMDNX_IF_ERR_EXIT(rv);
        if (!session_in_use) {
            break;
        }
    }

    if (session_index >= session_count) {
        session_index = -1;
    }

    return session_index;

    exit:
        BCMDNX_FUNC_RETURN;
}

/*
 * Functions:
 *      _bcm_petra_bfd_uc_<header/label>_pack
 * Purpose:
 *      The following set of _pack() functions packs in
 *      network byte order a given header/label.
 * Parameters:
 *      buffer          - (OUT) Buffer where to pack header/label.
 *      <header/label>  - (IN) Header/label to pack.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 */
STATIC uint8 *
_bcm_petra_bfd_uc_udp_header_pack(uint8 *buffer, bfd_udp_header_t *udp)
{
    _SHR_PACK_U16(buffer, udp->src);
    _SHR_PACK_U16(buffer, udp->dst);
    _SHR_PACK_U16(buffer, udp->length);
    _SHR_PACK_U16(buffer, udp->sum);

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_ipv4_header_pack(uint8 *buffer, bfd_ipv4_header_t *ip)
{
    uint32  tmp;

    tmp = ((ip->version & 0xf) << 28) | ((ip->h_length & 0xf) << 24) |
        ((ip->dscp & 0x3f) << 18) | ((ip->ecn & 0x3)<< 16) | (ip->length);
    _SHR_PACK_U32(buffer, tmp);

    tmp = (ip->id << 16) | ((ip->flags & 0x7) << 13) | (ip->f_offset & 0x1fff);
    _SHR_PACK_U32(buffer, tmp);

    tmp = (ip->ttl << 24) | (ip->protocol << 16) | ip->sum;
    _SHR_PACK_U32(buffer, tmp);

    _SHR_PACK_U32(buffer, ip->src);
    _SHR_PACK_U32(buffer, ip->dst);

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_ipv6_header_pack(uint8 *buffer, bfd_ipv6_header_t *ip)
{
    uint32  tmp;
    int     i;

    tmp = ((ip->version & 0xf) << 28) | (ip->t_class << 20) |
        (ip->f_label & 0xfffff);
    _SHR_PACK_U32(buffer, tmp);

    tmp = (ip->p_length << 16 ) | (ip->next_header << 8) | ip->hop_limit;
    _SHR_PACK_U32(buffer, tmp);

    for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
        _SHR_PACK_U8(buffer, ip->src[i]);
    }

    for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
        _SHR_PACK_U8(buffer, ip->dst[i]);
    }

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_gre_header_pack(uint8 *buffer, bfd_gre_header_t *gre)
{
    uint32  tmp;

    tmp = ((gre->checksum & 0x1) << 31) | ((gre->routing & 0x1) << 30) |
        ((gre->key & 0x1) << 29) | ((gre->sequence_num & 0x1) << 28) | 
        ((gre->strict_src_route & 0x1) << 27) |
        ((gre->recursion_control & 0x7) << 24) | ((gre->flags & 0x1f) << 19) |
        ((gre->version & 0x7) << 16) | (gre->protocol & 0xffff);

    _SHR_PACK_U32(buffer, tmp);

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_ach_header_pack(uint8 *buffer, bfd_ach_header_t *ach)
{
    uint32  tmp;

    tmp = ((ach->f_nibble & 0xf) << 28) | ((ach->version & 0xf) << 24) |
        (ach->reserved << 16) | ach->channel_type;

    _SHR_PACK_U32(buffer, tmp);

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_mpls_label_pack(uint8 *buffer, bfd_mpls_label_t *mpls)
{
    uint32  tmp;

    tmp = ((mpls->label & 0xfffff) << 12) | ((mpls->exp & 0x7) << 9) |
        ((mpls->s & 0x1) << 8) | mpls->ttl;
    _SHR_PACK_U32(buffer, tmp);

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_udh_header_pack(uint8 *buffer, bfd_udh_header_t *udh)
{
    int i;
    
    for (i = 0; i < udh->size; i++) {
        _SHR_PACK_U8(buffer, udh->value[i]);
    }

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_pph_header_pack(uint8 *buffer, bfd_pph_header_t *pph)
{
#if defined(LE_HOST)
    _SHR_PACK_U8(buffer, pph->raw.data[6]);
    _SHR_PACK_U8(buffer, pph->raw.data[5]);
    _SHR_PACK_U8(buffer, pph->raw.data[4]);
    _SHR_PACK_U8(buffer, pph->raw.data[3]);
    _SHR_PACK_U8(buffer, pph->raw.data[2]);
    _SHR_PACK_U8(buffer, pph->raw.data[1]);
    _SHR_PACK_U8(buffer, pph->raw.data[0]);
#else
    _SHR_PACK_U8(buffer, pph->raw.data[0]);
    _SHR_PACK_U8(buffer, pph->raw.data[1]);
    _SHR_PACK_U8(buffer, pph->raw.data[2]);
    _SHR_PACK_U8(buffer, pph->raw.data[3]);
    _SHR_PACK_U8(buffer, pph->raw.data[4]);
    _SHR_PACK_U8(buffer, pph->raw.data[5]);
    _SHR_PACK_U8(buffer, pph->raw.data[6]);
#endif

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_itmh_ext_header_pack(uint8 *buffer, bfd_itmh_ext_header_t *itmh_ext)
{
#if defined(LE_HOST)
    _SHR_PACK_U8(buffer, itmh_ext->raw.data[2]);
    _SHR_PACK_U8(buffer, itmh_ext->raw.data[1]);
    _SHR_PACK_U8(buffer, itmh_ext->raw.data[0]);
#else 
    _SHR_PACK_U8(buffer, itmh_ext->raw.data[0]);
    _SHR_PACK_U8(buffer, itmh_ext->raw.data[1]);
    _SHR_PACK_U8(buffer, itmh_ext->raw.data[2]);
#endif

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_itmh_header_pack(uint8 *buffer, bfd_itmh_header_t *itmh)
{
    _SHR_PACK_U32(buffer, itmh->raw.data);

    return buffer;
}

STATIC uint8 *
_bcm_petra_bfd_uc_ptch2_header_pack(uint8 *buffer, bfd_ptch2_header_t *ptch2)
{
    _SHR_PACK_U16(buffer, ptch2->raw.data);

    return buffer;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_udp_checksum_set
 * Purpose:
 *      Set the UDP initial checksum (excludes checksum for the data).
 *
 *      The checksum includes the IP pseudo-header and UDP header.
 *      It does not include the checksum for the data (BFD packet).
 *      The data checksum will be added to UDP initial checksum in
 *      the uC side each time a packet is sent, since data payload may vary.
 * Parameters:
 *      v6              - (IN) Set if IP is IPv6.
 *      ipv4            - (IN) IPv4 header.
 *      ipv6            - (IN) IPv6 header.
 *      udp             - (IN/OUT) UDP header checksum to update.
 * Returns:
 *      None
 */
STATIC void
_bcm_petra_bfd_uc_udp_checksum_set(int v6,
                               bfd_ipv4_header_t *ipv4,
                               bfd_ipv6_header_t *ipv6,
                               bfd_udp_header_t *udp)
{
    uint8  buffer[SHR_BFD_UDP_HEADER_LENGTH + (BCM_IP6_ADDRLEN*2) + 8];
    uint8  *cur_ptr = buffer;
    int    i, length;

    /* Build IP Pseudo-Header */
    if (v6) {
        /* IPv6 Pseudo-Header
         *     Source address
         *     Destination address
         *     UDP length  (32-bit)
         *     Next Header (lower 8 bits of 32-bit)
         */
        for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
            _SHR_PACK_U8(cur_ptr, ipv6->src[i]);
        }
        for (i = 0; i < BCM_IP6_ADDRLEN; i++) {
            _SHR_PACK_U8(cur_ptr, ipv6->dst[i]);
        }
        _SHR_PACK_U16(cur_ptr, udp->length);
        _SHR_PACK_U8(cur_ptr, 0); /* Reserved */
        _SHR_PACK_U8(cur_ptr, ipv6->next_header);
    } else {
        /* IPv4 Pseudo-Header
         *     Source address
         *     Destination address
         *     Protocol    (8 bits)
         *     UDP length  (16-bit)
         */
        _SHR_PACK_U32(cur_ptr, ipv4->src);
        _SHR_PACK_U32(cur_ptr, ipv4->dst);
        _SHR_PACK_U8(cur_ptr, 0); /* Reserved */
        _SHR_PACK_U8(cur_ptr, ipv4->protocol);
        _SHR_PACK_U16(cur_ptr, udp->length);
    }

    /* Add UDP header */
    cur_ptr = _bcm_petra_bfd_uc_udp_header_pack(cur_ptr, udp);

    /* Calculate initial UDP checksum */
    length = cur_ptr - buffer;
    udp->sum = _shr_ip_chksum(length, buffer);
}

/*
 * Functions:
 *      _bcm_petra_bfd_uc_<header/label>_get
 * Purpose:
 *      The following set of _get() functions builds a given header/label.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN) Pointer to BFD endpoint structure.
 *      packet_flags    - (IN) Flags for building packet.
 *      <... other IN args ...>
 *      <header/label>  - (OUT) Returns header/label.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_petra_bfd_uc_udp_header_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                             uint32 packet_flags, bfd_udp_header_t *udp)
{

    int auth_length;
    int bfd_length;
    int rv = 0;
    bcm_bfd_auth_simple_password_t auth_sp;

    BCMDNX_INIT_FUNC_DEFS;

    /* Calculate BFD packet length */
    /* Authentication (optional) */
    switch (endpoint_info->auth) {
    case _SHR_BFD_AUTH_TYPE_SIMPLE_PASSWORD:
        rv = BFDIPV6_ACCESS.auth_sp.get(unit, endpoint_info->auth_index, &auth_sp);
        BCMDNX_IF_ERR_EXIT(rv);
        auth_length = SHR_BFD_AUTH_SP_HEADER_START_LENGTH +
            auth_sp.length;
        break;
    case _SHR_BFD_AUTH_TYPE_KEYED_SHA1:
    case _SHR_BFD_AUTH_TYPE_METICULOUS_KEYED_SHA1:
        auth_length = SHR_BFD_AUTH_SHA1_LENGTH;
        break;
    default:
        auth_length = 0;
        break;
    }
    bfd_length = SHR_BFD_CONTROL_HEADER_LENGTH + auth_length;

    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = endpoint_info->udp_src_port;
    udp->dst = (packet_flags & _BCM_BFD_ENCAP_PKT_UDP__MULTI_HOP) ?
        SHR_BFD_UDP_MULTI_HOP_DEST_PORT : 
        ((endpoint_info->flags & BCM_BFD_ENDPOINT_MICRO_BFD)
        ? SHR_MICRO_BFD_DEST_PORT : SHR_BFD_UDP_SINGLE_HOP_DEST_PORT);
    udp->length = SHR_BFD_UDP_HEADER_LENGTH + bfd_length;
    udp->sum = 0;    /* Calculated later */

    exit:
        BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_bfd_uc_ipv4_header_get(int data_length, uint8 protocol,
                              uint8 tos, uint8 ttl,
                              bcm_ip_t src_ip_addr, bcm_ip_t dst_ip_addr,
                              bfd_ipv4_header_t *ip)
{
    uint8 buffer[SHR_BFD_IPV4_HEADER_LENGTH];

    sal_memset(ip, 0, sizeof(*ip));

    ip->version  = SHR_BFD_IPV4_VERSION;
    ip->h_length = SHR_BFD_IPV4_HEADER_LENGTH_WORDS;
    ip->dscp     = tos >> 2;
    ip->ecn      = tos & 0x3;
    ip->length   = SHR_BFD_IPV4_HEADER_LENGTH + data_length;
    ip->id       = 0;
    ip->flags    = 0;
    ip->f_offset = 0;
    ip->ttl      = ttl;
    ip->protocol = protocol;
    ip->sum      = 0;
    ip->src      = src_ip_addr;
    ip->dst      = dst_ip_addr;

    /* Calculate IP header checksum */
    _bcm_petra_bfd_uc_ipv4_header_pack(buffer, ip);
    ip->sum = _shr_ip_chksum(SHR_BFD_IPV4_HEADER_LENGTH, buffer);

    return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_ipv6_header_get(int data_length, uint8 protocol,
                              uint8 tos, uint8 ttl,
                              bcm_ip6_t src_ip_addr, bcm_ip6_t dst_ip_addr,
                              bfd_ipv6_header_t *ip)
{
    sal_memset(ip, 0, sizeof(*ip));

    ip->version     = SHR_BFD_IPV6_VERSION;
    ip->t_class     = tos;
    ip->f_label     = 0;  
    ip->p_length    = data_length;
    ip->next_header = protocol;
    ip->hop_limit   = ttl;
    sal_memcpy(ip->src, src_ip_addr, BCM_IP6_ADDRLEN);
    sal_memcpy(ip->dst, dst_ip_addr, BCM_IP6_ADDRLEN);

    return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_ip_headers_get(bcm_bfd_endpoint_info_t *endpoint_info,
                             uint32 packet_flags,
                             int udp_length,
                             uint16 *etype,
                             bfd_ipv4_header_t *ipv4, bfd_ipv6_header_t *ipv6,
                             bfd_ipv4_header_t *inner_ipv4,
                             bfd_ipv6_header_t *inner_ipv6)
{
    int ip_protocol;
    int ip_length;

    if (!(packet_flags & _BCM_BFD_ENCAP_PKT_IP)) {
        return BCM_E_PARAM;
    }

    /* Inner IP Header (IP-in-IP/GRE) */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP) {

        if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP__V6) {
            ip_protocol = SHR_BFD_IP_PROTOCOL_IPV6_ENCAP;
            ip_length   = udp_length + SHR_BFD_IPV6_HEADER_LENGTH;
            BCM_IF_ERROR_RETURN
                (_bcm_petra_bfd_uc_ipv6_header_get
                 (udp_length,
                  SHR_BFD_IP_PROTOCOL_UDP,
                  endpoint_info->inner_ip_tos,
                  endpoint_info->inner_ip_ttl,
                  endpoint_info->inner_src_ip6_addr,
                  endpoint_info->inner_dst_ip6_addr,
                  inner_ipv6));
        } else {
            ip_protocol = SHR_BFD_IP_PROTOCOL_IPV4_ENCAP;
            ip_length   = udp_length + SHR_BFD_IPV4_HEADER_LENGTH;
            BCM_IF_ERROR_RETURN
                (_bcm_petra_bfd_uc_ipv4_header_get
                 (udp_length,
                  SHR_BFD_IP_PROTOCOL_UDP,
                  endpoint_info->inner_ip_tos,
                  endpoint_info->inner_ip_ttl,
                  endpoint_info->inner_src_ip_addr,
                  endpoint_info->inner_dst_ip_addr,
                  inner_ipv4));
        }
    } else {
        ip_protocol = SHR_BFD_IP_PROTOCOL_UDP;
        ip_length   = udp_length;
    }

    /* If GRE is present, GRE the protocol that follows the outer IP header */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_GRE) {
        ip_protocol = SHR_BFD_IP_PROTOCOL_GRE;
        ip_length   = udp_length + SHR_BFD_GRE_HEADER_LENGTH;
        if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP__V6) {
            ip_length += SHR_BFD_IPV6_HEADER_LENGTH;
        } else {
            ip_length += SHR_BFD_IPV4_HEADER_LENGTH;
        }
    }

    /* Outer IP Header */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_IP__V6) {
        *etype = SHR_BFD_L2_ETYPE_IPV6;
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_ipv6_header_get(ip_length,
                                           ip_protocol,
                                           endpoint_info->ip_tos,
                                           endpoint_info->ip_ttl,
                                           endpoint_info->src_ip6_addr,
                                           endpoint_info->dst_ip6_addr,
                                           ipv6));
    } else {
        *etype = SHR_BFD_L2_ETYPE_IPV4;
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_ipv4_header_get(ip_length,
                                           ip_protocol,
                                           endpoint_info->ip_tos,
                                           endpoint_info->ip_ttl,
                                           endpoint_info->src_ip_addr,
                                           endpoint_info->dst_ip_addr,
                                           ipv4));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_gre_header_get(uint32 packet_flags, bfd_gre_header_t *gre)
{
    sal_memset(gre, 0, sizeof(*gre));

    gre->checksum          = 0;
    gre->routing           = 0;
    gre->key               = 0;
    gre->sequence_num      = 0;
    gre->strict_src_route  = 0;
    gre->recursion_control = 0;
    gre->flags             = 0;
    gre->version           = 0;
    if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP__V6) {
        gre->protocol = SHR_BFD_L2_ETYPE_IPV6;
    } else {
        gre->protocol = SHR_BFD_L2_ETYPE_IPV4;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_ach_header_get(uint32 packet_flags, bfd_ach_header_t *ach)
{
    sal_memset(ach, 0, sizeof(*ach));

    ach->f_nibble = SHR_BFD_ACH_FIRST_NIBBLE;
    ach->version  = SHR_BFD_ACH_VERSION;
    ach->reserved = 0;

    if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH__IP) {    /* UDP/IP */
        if (packet_flags & _BCM_BFD_ENCAP_PKT_IP__V6) {
            ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_IPV6;
        } else {
            ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_IPV4;
        }

    } else if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH__CC) { /* MPLS-TP CC only */
        ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_MPLS_TP_CC;

    } else if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH__CC_CV) { /* MPLS-TP CC/CV */
        /*
         * CC/CV is done by interleaving CC and CV PDUs.
         * Set CC channel type as a start.
         * BFD engine (uKernel) will set appropriate channel types.
         * One CV PDU is inserted every second.
         */
        ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_MPLS_TP_CC;

    } else {    /* Raw */
        ach->channel_type = SHR_BFD_ACH_CHANNEL_TYPE_RAW;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_mpls_label_get(uint32 label, uint8 exp, uint8 s, uint8 ttl,
                             bfd_mpls_label_t *mpls)
{
    sal_memset(mpls, 0, sizeof(*mpls));

    mpls->label = label;
    mpls->exp   = exp;
    mpls->s     = s;
    mpls->ttl   = ttl;

    return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_mpls_gal_label_get(bfd_mpls_label_t *mpls)
{
    return _bcm_petra_bfd_uc_mpls_label_get(SHR_BFD_MPLS_GAL_LABEL,
                                        0, 1, 1, mpls);
}

STATIC int
_bcm_petra_bfd_uc_mpls_labels_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info,
                              uint32 packet_flags,
                              int max_count, bfd_mpls_label_t *mpls,
                              int *mpls_count)
{
    return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_udh_header_get(int unit, bfd_udh_header_t *udh)
{
    uint32
    res = SOC_SAND_OK,
    user_header_0_size,
    user_header_1_size,
    user_header_egress_pmf_offset_0,
    user_header_egress_pmf_offset_1;
    int i;
    
    BCMDNX_INIT_FUNC_DEFS;

    udh->size = 0;
    /* if TM packet should be injected with UDH we increase the ITMH header size of the dsp packets*/
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "injection_with_user_header_enable", 0 /*DEFAULT VALUE*/) == 1) {

        res = arad_pmf_db_fes_user_header_sizes_get(
            unit,
            &user_header_0_size,
            &user_header_1_size,
            &user_header_egress_pmf_offset_0,
            &user_header_egress_pmf_offset_1
            );

        user_header_0_size /= 8;
        user_header_1_size /= 8;
        BCMDNX_IF_ERR_EXIT(res);    
        udh->size = user_header_0_size + user_header_1_size;
        for (i = 0; i < udh->size; i++) {
            udh->value[i] = 0;
        }
    }    

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_bfd_uc_ptch2_header_get(int unit, bfd_ptch2_header_t *ptch2)
{
    ptch2->arad_common.parser_program_control = 0;
    ptch2->arad_common.opaque_pt_attributes = 7;
    ptch2->arad_common.reserved = 0;
    ptch2->arad_common.pp_ssp = soc_property_get(unit, spn_BFD_IPV6_SOURCE_CPU_PORT, 0);

	return BCM_E_NONE;
}

STATIC int
_bcm_petra_bfd_uc_itmh_header_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info, bfd_itmh_header_t *itmh)
{   
    BCMDNX_INIT_FUNC_DEFS;
    
    if(SOC_IS_JERICHO(unit)) { 
        uint32 tx_port;
        
        itmh->jericho_common.pph_type = 1; 
        itmh->jericho_common.in_mirror_disable = 0;
        itmh->jericho_common.fwd_dp = (endpoint_info->int_pri & 0x3);
        itmh->jericho_common.snoop_cmp = 0;
        itmh->jericho_common.fwd_traffic_class = ((endpoint_info->int_pri >> 2) & 0x7);
        itmh->jericho_common.itmh_base_extension_exists = 1;  
        
        if (_bcm_dpp_oam_bfd_tx_gport_to_sys_port(unit, endpoint_info->tx_gport, &tx_port)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported tx_gport.")));
        }
        itmh->jericho_common.fwd_dest_info = (1 << 16) | tx_port;
    }else {
        itmh->arad_common.pph_type = 1; 
        itmh->arad_common.in_mirror_flag = 0;
        itmh->arad_common.snoop_cmp = 0;
        itmh->arad_common.fwd_traffic_class = ((endpoint_info->int_pri >> 2) & 0x7);
        itmh->arad_common.fwd_dp = (endpoint_info->int_pri & 0x3);
        if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->egress_if)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported egress_if.")));
        }
        itmh->arad_common.outlif = 0xA << 16 | BCM_L3_ITF_VAL_GET(endpoint_info->egress_if);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_bfd_uc_itmh_ext_header_get(int unit, bcm_bfd_endpoint_info_t *endpoint_info, bfd_itmh_ext_header_t *itmh_ext)
{
   
    BCMDNX_INIT_FUNC_DEFS;
    if(SOC_IS_JERICHO(unit)) {
        itmh_ext->jericho_common.reserved = 0;
        itmh_ext->jericho_common.fwd_dest_info_type = 0;
        if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->egress_if)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported egress_if.")));
        }
        itmh_ext->jericho_common.fwd_dest_info = BCM_L3_ITF_VAL_GET(endpoint_info->egress_if);
    }else {
        uint32 tx_port;
        if (_bcm_dpp_oam_bfd_tx_gport_to_sys_port(unit, endpoint_info->tx_gport, &tx_port)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: Unsupported tx_gport.")));
        }
        itmh_ext->arad_common.reserved = 0;
        itmh_ext->arad_common.fwd_dest_info_type = 0xC;
        itmh_ext->arad_common.fwd_dest_info = tx_port;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_bfd_uc_pph_header_get(int unit, bfd_udh_header_t *udh, bfd_pph_header_t *pph)
{
    pph->arad_common.eei_ext_present = 0;
    pph->arad_common.learn_ext_present = 0;
    pph->arad_common.fhei_size = 0;
    pph->arad_common.fwd_code = 3; /* IPv6-UC-Routed */
    pph->arad_common.fwd_header_offset = udh->size;
    pph->arad_common.pkt_is_ctrl = 0;
    pph->arad_common.snoop_cpu_code = 0;
    pph->arad_common.inlif_orientiation = 0;
    pph->arad_common.unknown_da = 0;
    pph->arad_common.learn_allowed = 0;
    pph->arad_common.vsi_vrf_hi = 0;
    pph->arad_common.vsi_vrf_lo = 0;
    pph->arad_common.inlif_inrif = 0;

	return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_petra_bfd_uc_encap_build_pack
 * Purpose:
 *      Builds and packs the BFD packet encapsulation for a given
 *      BFD tunnel type.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN/OUT) Pointer to BFD endpoint structure.
 *      packet_flags    - (IN) Flags for building packet.
 *      buffer          - (OUT) Buffer returning BFD encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BFD encapsulation includes only all the
 *      encapsulation headers/labels and does not include
 *      the BFD control packet.
 */
STATIC int
_bcm_petra_bfd_uc_encap_build_pack(int unit,
                               bfd_endpoint_config_t *endpoint_config,
                               uint32 packet_flags,
                               uint8 *buffer)
{
    bfd_endpoint_info_t *endpoint_info = &endpoint_config->info;
    bcm_bfd_endpoint_info_t temp_info;
    uint8          *cur_ptr = buffer;
    uint16         etype = 0;
    bfd_udp_header_t  udp;
    bfd_ipv4_header_t ipv4, inner_ipv4;
    bfd_ipv6_header_t ipv6, inner_ipv6;
    bfd_gre_header_t  gre;
    int            v6;
    bfd_ach_header_t  ach;
    bfd_mpls_label_t  mpls_gal;
    bfd_mpls_label_t  mpls_labels[_BCM_BFD_MPLS_MAX_LABELS];
    int            mpls_count = 0;
    int            i;
	bfd_udh_header_t udh;
    bfd_pph_header_t pph;
    bfd_itmh_ext_header_t itmh_ext;
    bfd_itmh_header_t itmh;
    bfd_ptch2_header_t ptch2;
    int            ip_offset = 0;

    sal_memset(&ptch2, 0, sizeof(ptch2));
	sal_memset(&itmh, 0, sizeof(itmh));
	sal_memset(&itmh_ext, 0, sizeof(itmh_ext));
	sal_memset(&pph, 0, sizeof(pph));
	sal_memset(&udh, 0, sizeof(udh));
	sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&inner_ipv4, 0, sizeof(inner_ipv4));
    sal_memset(&ipv6, 0, sizeof(ipv6));
    sal_memset(&inner_ipv6, 0, sizeof(inner_ipv6));

    bcm_petra_memcpy_to_bcm_bfd_endpoint_info(&temp_info, endpoint_info);
    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_UDP) {
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_udp_header_get(unit, &temp_info,
                                          packet_flags, &udp));
    }

    if (packet_flags & _BCM_BFD_ENCAP_PKT_IP) {
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_ip_headers_get(&temp_info,
                                          packet_flags, udp.length,
                                          &etype,
                                          &ipv4, &ipv6,
                                          &inner_ipv4, &inner_ipv6));
    }

    /* Set UDP checksum with corresponding IP header information */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_UDP) {
        v6 = 0;
        if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP) {
            if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP__V6) {
                v6 = 1;
            }
            _bcm_petra_bfd_uc_udp_checksum_set(v6, &inner_ipv4, &inner_ipv6, &udp);
        } else {
            if (packet_flags & _BCM_BFD_ENCAP_PKT_IP__V6) {
                v6 = 1;
            }
            _bcm_petra_bfd_uc_udp_checksum_set(v6, &ipv4, &ipv6, &udp);
        }
    }

    if (packet_flags & _BCM_BFD_ENCAP_PKT_GRE) {
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_gre_header_get(packet_flags, &gre));
    }

    if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH) {
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_ach_header_get(packet_flags, &ach));
    }

    if (packet_flags & _BCM_BFD_ENCAP_PKT_GAL) {
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_mpls_gal_label_get(&mpls_gal));
    }

    if (packet_flags & _BCM_BFD_ENCAP_PKT_MPLS) {
        etype = SHR_BFD_L2_ETYPE_MPLS_UCAST;
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_mpls_labels_get(unit, &temp_info,
                                           packet_flags,
                                           _BCM_BFD_MPLS_MAX_LABELS,
                                           mpls_labels, &mpls_count));

        /* User explicitly set the MPLS egress label exp and ttl values */
        if (temp_info.egress_label.exp != 0xFF) {
            if (temp_info.egress_label.exp <= 0x07) {
                mpls_labels[0].exp = temp_info.egress_label.exp;
            } else { 
                return BCM_E_PARAM;
            }
        }
        if (temp_info.egress_label.ttl != 0) {
           mpls_labels[0].ttl = temp_info.egress_label.ttl;
        }
    }

    /* Always build UDH Header */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_udh_header_get(unit, &udh));

    /* Always build PPH Header */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_pph_header_get(unit, &udh, &pph));

    /* Always build ITMH extension Header */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_itmh_ext_header_get(unit, &temp_info, &itmh_ext));

    /* Always build ITMH Header */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_itmh_header_get(unit, &temp_info, &itmh));
    
    /* Always build PTCH2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_ptch2_header_get(unit, &ptch2));

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;
    if (cur_ptr == NULL) {
        return BCM_E_INTERNAL;
    }

    /* PTCH-2 Header is always present */
    cur_ptr = _bcm_petra_bfd_uc_ptch2_header_pack(cur_ptr, &ptch2);

    /* ITMH Header is always present */
    cur_ptr = _bcm_petra_bfd_uc_itmh_header_pack(cur_ptr, &itmh);
    
    /* ITMH extension Header is always present */
    cur_ptr = _bcm_petra_bfd_uc_itmh_ext_header_pack(cur_ptr, &itmh_ext);

    /* PPH Header is always present */
    cur_ptr = _bcm_petra_bfd_uc_pph_header_pack(cur_ptr, &pph);

    /* UDH Header is always present */
    cur_ptr = _bcm_petra_bfd_uc_udh_header_pack(cur_ptr, &udh);

    if (packet_flags & _BCM_BFD_ENCAP_PKT_MPLS) {
        for (i = 0; i < mpls_count; i++) {
            cur_ptr = _bcm_petra_bfd_uc_mpls_label_pack(cur_ptr, &mpls_labels[i]);
        }
    }

    /* Remaining packet headers */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_GAL) {
        cur_ptr = _bcm_petra_bfd_uc_mpls_label_pack(cur_ptr, &mpls_gal);
    }
    if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH) {
        cur_ptr = _bcm_petra_bfd_uc_ach_header_pack(cur_ptr, &ach);
    }
    if (packet_flags & _BCM_BFD_ENCAP_PKT_GRE) {
    }
    if (packet_flags & _BCM_BFD_ENCAP_PKT_IP) {
        /*
         * IP offset for Lookup Key
         * For IP-in-IP, this is the outer IP header
         */
        ip_offset = cur_ptr - buffer;
        if (packet_flags & _BCM_BFD_ENCAP_PKT_IP__V6) {
            cur_ptr = _bcm_petra_bfd_uc_ipv6_header_pack(cur_ptr, &ipv6);
        } else {
            cur_ptr = _bcm_petra_bfd_uc_ipv4_header_pack(cur_ptr, &ipv4);
        }
    }
    if (packet_flags & _BCM_BFD_ENCAP_PKT_GRE) {
        cur_ptr = _bcm_petra_bfd_uc_gre_header_pack(cur_ptr, &gre);
    }
    if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP) {
        if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP__V6) {
            cur_ptr = _bcm_petra_bfd_uc_ipv6_header_pack(cur_ptr, &inner_ipv6);
        } else {
            cur_ptr = _bcm_petra_bfd_uc_ipv4_header_pack(cur_ptr, &inner_ipv4);
        }
    }
    if (packet_flags & _BCM_BFD_ENCAP_PKT_UDP) {
        cur_ptr = _bcm_petra_bfd_uc_udp_header_pack(cur_ptr, &udp);
    }


    /* Set BFD encapsulation length */
    endpoint_config->encap_length = cur_ptr - buffer;

    /*
     * Set Lookup Key for initial BFD packets
     * Offset is relative to the start of a given encapsulation data.
     *
     * Lookup Key:
     *   UDP/IP  : IPSA of RX packet       = IPDA on BFD Session
     *   IP-in-IP: Outer IPSA of RX packet = Outer IPDA on BFD Session
     *   MPLS    : MPLS BOS of RX packet   = 'label' on BFD endpoint struct
     *             - If GAL label (BOS) is present, then use MPLS label above.
     *             - 'mpls_label' on BFD endpoint is the incomming inner MPLS
     *               label.
     */
    endpoint_config->lkey_etype  = etype;
    endpoint_config->lkey_offset = 0;
    endpoint_config->lkey_length = 0;

    if (etype == SHR_BFD_L2_ETYPE_IPV4) {
        endpoint_config->lkey_offset = ip_offset +
            SHR_BFD_IPV4_HEADER_DA_OFFSET;
        endpoint_config->lkey_length = SHR_BFD_IPV4_HEADER_SA_LENGTH;

    } else if (etype ==  SHR_BFD_L2_ETYPE_IPV6) {
        endpoint_config->lkey_offset = ip_offset +
            SHR_BFD_IPV6_HEADER_DA_OFFSET;
        endpoint_config->lkey_length = SHR_BFD_IPV6_HEADER_SA_LENGTH;

    } else if (etype == SHR_BFD_L2_ETYPE_MPLS_UCAST) {
        /*
         * The lookup key is the incomming inner MPLS label,
         * which is specified when the BFD endpoint is create.
         *
         * The lookup key data is located in the 'mpls_label'
         * field of the message, rather than in the encap data
         * packet buffer.
         */
        endpoint_config->lkey_offset = 0;
        endpoint_config->lkey_length = SHR_BFD_MPLS_LABEL_LABEL_LENGTH;
    }

    bcm_petra_memcpy_to_bfd_endpoint_info(endpoint_info, &temp_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_encap_data_dump
 * Purpose:
 *      Dumps buffer contents.
 * Parameters:
 *      buffer  - (IN) Buffer to dump data.
 *      length  - (IN) Length of buffer.
 * Returns:
 *      None
 */
void
_bcm_petra_bfd_uc_encap_data_dump(uint8 *buffer, int length)
{
    int i;

    LOG_CLI((BSL_META("\nBFD encapsulation (length=%d):\n"), length));

    for (i = 0; i < length; i++) {
        if ((i % 10) == 0) {
            LOG_CLI((BSL_META("\n")));
        }
        LOG_CLI((BSL_META(" 0x%02x"), buffer[i]));
    }

    LOG_CLI((BSL_META("\n")));
    return;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_ip_validate_1
 * Purpose:
 *      Performs the following IP address check and IP TTL configuration:
 *      - IP Source Address is routable
 *      - IP Destination Addresss is routable
 *      - Sets IP TTL=255  for 1-hop
 * Parameters:
 *      endpoint_info - (IN/OUT) Pointer to BFD endpoint info structure.
 *      packet_flags  - (IN) Packet flags.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_bfd_uc_ip_validate_1(bcm_bfd_endpoint_info_t *endpoint_info,
                            uint32 packet_flags)
{

    /* IP-SA and IP-DA Routable */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_IP__V6) {
        if (BCM_IP6_MULTICAST(endpoint_info->src_ip6_addr)) {
            return BCM_E_PARAM;
        }
        if (IPV6_LOOPBACK(endpoint_info->dst_ip6_addr)) {
            return BCM_E_PARAM;
        }
    } else {
        if (BCM_IP4_MULTICAST(endpoint_info->src_ip_addr)) {
            return BCM_E_PARAM;
        }
        if (IPV4_LOOPBACK(endpoint_info->dst_ip_addr)) {
            return BCM_E_PARAM;
        }
    }

    if (!(packet_flags & _BCM_BFD_ENCAP_PKT_UDP__MULTI_HOP)) {
        endpoint_info->ip_ttl = 255;    /* 1-hop */
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_ip_validate_2
 * Purpose:
 *      Performs the following IP address check and IP TTL configuration:
 *      - IP Source Address is routable
 *      - IP Destination Addresss is loopback or routable address
 *      - Sets IP TTL=1  for 1-hop when IP DA is loopback,
 *        or   IP TTL=255 for 1-hop when IP DA is a routable address         
 * Parameters:
 *      endpoint_info - (IN/OUT) Pointer to BFD endpoint info structure.
 *      packet_flags  - (IN) Packet flags.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_petra_bfd_uc_ip_validate_2(bcm_bfd_endpoint_info_t *endpoint_info,
                            uint32 packet_flags)
{
    int ip_ttl = 0;
    bcm_ip_t dst_ip_addr;
    bcm_ip6_t src_ip6_addr, dst_ip6_addr;
    int v6 = 0;

    if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP) {
        if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP__V6) {
            v6 = 1;
        }
        dst_ip_addr  = endpoint_info->inner_dst_ip_addr;
        sal_memcpy(src_ip6_addr, endpoint_info->inner_src_ip6_addr,
                   BCM_IP6_ADDRLEN);
        sal_memcpy(dst_ip6_addr, endpoint_info->inner_dst_ip6_addr,
                   BCM_IP6_ADDRLEN);
    } else {
        if (packet_flags & _BCM_BFD_ENCAP_PKT_IP__V6) {
            v6 = 1;
        }
        dst_ip_addr  = endpoint_info->dst_ip_addr;
        sal_memcpy(src_ip6_addr, endpoint_info->src_ip6_addr,
                   BCM_IP6_ADDRLEN);
        sal_memcpy(dst_ip6_addr, endpoint_info->dst_ip6_addr,
                   BCM_IP6_ADDRLEN);
    }

    /* IP-DA:
     *     Loopback: 127/8 or 0:0:0:0:0:FFFF:127/104, then ttl=1
     *     Routable: then ttl=255
     */
    if (v6) {
        if (IPV6_LOOPBACK(dst_ip6_addr)) {
            ip_ttl = 1;
        } else {
            ip_ttl = 255;
        }
    } else {
        if (IPV4_LOOPBACK(dst_ip_addr)) {
            ip_ttl = 1;
        } else {
            ip_ttl = 255;
        }
    }

    if (!(packet_flags & _BCM_BFD_ENCAP_PKT_UDP__MULTI_HOP)) {
        /* 1-hop */
        if (packet_flags & _BCM_BFD_ENCAP_PKT_INNER_IP) {
            endpoint_info->inner_ip_ttl = ip_ttl;
        } else {
            endpoint_info->ip_ttl = ip_ttl;
        }            
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_petra_bfd_uc_encap_create
 * Purpose:
 *      Creates a BFD packet encapsulation.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      endpoint_config - (IN/OUT) Pointer to BFD endpoint structure.
 *      encap_data      - (OUT) Buffer returning BFD encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning BFD encapsulation buffer includes all the
 *      corresponding headers/labels EXCEPT for the BFD control packet.
 */
STATIC int
_bcm_petra_bfd_uc_encap_create(int unit,
                           bfd_endpoint_config_t *endpoint_config,
                           uint8 *encap_data)
{
    bcm_bfd_endpoint_info_t *endpoint_info;
    uint32 packet_flags;
    uint32 ip_flag;
    bcm_port_t tx_port;  /* Local port to TX packet */
    bcm_pbmp_t st_pbmp;
    bcm_pbmp_t tx_pbmp;

    endpoint_info = (bcm_bfd_endpoint_info_t*)&endpoint_config->info;
    tx_port       = endpoint_config->tx_port;

    /* Check for incompatible flags */
    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW) &&
        (endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6)) {
        return BCM_E_PARAM;
    }

    packet_flags = 0;
    ip_flag      = 0;

    /* Set IPv4 or IPv6 flag */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_IPV6) {
        ip_flag |= _BCM_BFD_ENCAP_PKT_IP__V6;
    }

    /* 1-hop or Multi-hop */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
        packet_flags |= _BCM_BFD_ENCAP_PKT_UDP__MULTI_HOP;
    }

    /*
     * Get BFD encapsulation packet format flags
     *
     * Also, perform the following for each BFD tunnel type:
     * - Check for valid parameter values
     * - Set specific values required by the BFD tunnel definition 
     *   (e.g. such as ttl=1,...)
     */
    switch (endpoint_info->type) {
    case bcmBFDTunnelTypeUdp:
        packet_flags |= 
            (_BCM_BFD_ENCAP_PKT_IP | ip_flag |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeIp4in4:
        packet_flags |= 
            (_BCM_BFD_ENCAP_PKT_IP |
             _BCM_BFD_ENCAP_PKT_INNER_IP |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;


    case bcmBFDTunnelTypeIp6in4:
        packet_flags |= 
            (_BCM_BFD_ENCAP_PKT_IP |
             _BCM_BFD_ENCAP_PKT_INNER_IP | _BCM_BFD_ENCAP_PKT_INNER_IP__V6 |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeIp4in6:
        packet_flags |= 
            (_BCM_BFD_ENCAP_PKT_IP | _BCM_BFD_ENCAP_PKT_IP__V6 |
             _BCM_BFD_ENCAP_PKT_INNER_IP |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeIp6in6:
        packet_flags |= 
            (_BCM_BFD_ENCAP_PKT_IP | _BCM_BFD_ENCAP_PKT_IP__V6 |
             _BCM_BFD_ENCAP_PKT_INNER_IP | _BCM_BFD_ENCAP_PKT_INNER_IP__V6 |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeGre4In4:
        /*
         * bcmBFDTunnelTypeGRE is an old enum, which is now equivalent
         * to the new GRE IPv4-in-IPv4 tunnel type bcmBFDTunnelTypeGre4In4.
         */
        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_GRE |
             _BCM_BFD_ENCAP_PKT_IP |
             _BCM_BFD_ENCAP_PKT_INNER_IP |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeGre6In4:
        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_GRE |
             _BCM_BFD_ENCAP_PKT_IP |
             _BCM_BFD_ENCAP_PKT_INNER_IP | _BCM_BFD_ENCAP_PKT_INNER_IP__V6 |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeGre4In6:
        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_GRE |
             _BCM_BFD_ENCAP_PKT_IP | _BCM_BFD_ENCAP_PKT_IP__V6 |
             _BCM_BFD_ENCAP_PKT_INNER_IP |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeGre6In6:

        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_GRE |
             _BCM_BFD_ENCAP_PKT_IP | _BCM_BFD_ENCAP_PKT_IP__V6 |
             _BCM_BFD_ENCAP_PKT_INNER_IP | _BCM_BFD_ENCAP_PKT_INNER_IP__V6 |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_1(endpoint_info,
                                                        packet_flags));
        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));
        break;

    case bcmBFDTunnelTypeMpls:
        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_MPLS |
             _BCM_BFD_ENCAP_PKT_IP | ip_flag |
             _BCM_BFD_ENCAP_PKT_UDP);

        _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);

        BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                        packet_flags));

        break;

    case bcmBFDTunnelTypeMplsTpCc:
        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_MPLS |
             _BCM_BFD_ENCAP_PKT_G_ACH |
             _BCM_BFD_ENCAP_PKT_G_ACH__CC);

        if (endpoint_config->is_l3_vpn) {
            packet_flags |= _BCM_BFD_ENCAP_PKT_GAL;
        }

        break;

    case bcmBFDTunnelTypeMplsTpCcCv:
        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_MPLS |
             _BCM_BFD_ENCAP_PKT_G_ACH |
             _BCM_BFD_ENCAP_PKT_G_ACH__CC_CV);

        if (endpoint_config->is_l3_vpn) {
            packet_flags |= _BCM_BFD_ENCAP_PKT_GAL;
        }

        if ((endpoint_info->mep_id_length <= 0) ||
            (endpoint_info->mep_id_length >
             _SHR_BFD_ENDPOINT_MAX_MEP_ID_LENGTH)) {
            return BCM_E_PARAM;
        }
        break;

    case bcmBFDTunnelTypePweControlWord:
        packet_flags |=
            (_BCM_BFD_ENCAP_PKT_MPLS |
             _BCM_BFD_ENCAP_PKT_G_ACH);

        /* Check for incompatible flags */
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND) ||
            (endpoint_info->flags & BCM_BFD_ENDPOINT_PASSIVE)) {
            return BCM_E_PARAM;
        }

        /* PW required */
        if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
            return BCM_E_PARAM;
        }

        /* Raw mode or UDP/IP */
        if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW)) {
            /* UDP/IP */
            _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);
            packet_flags |= _BCM_BFD_ENCAP_PKT_G_ACH__IP;
            packet_flags |=
                (_BCM_BFD_ENCAP_PKT_IP | ip_flag |
                 _BCM_BFD_ENCAP_PKT_UDP);

            BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                            packet_flags));
        }
        break;

    case bcmBFDTunnelTypePweRouterAlert:
        packet_flags |= _BCM_BFD_ENCAP_PKT_MPLS_ROUTER_ALERT;
        /* Fall through */
    case bcmBFDTunnelTypePweTtl:
        packet_flags |=
            _BCM_BFD_ENCAP_PKT_MPLS;

        /* Check for incompatible flags */
        if ((endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND) ||
            (endpoint_info->flags & BCM_BFD_ENDPOINT_PASSIVE)) {
            return BCM_E_PARAM;
        }

        /* PW required */
        if (!BCM_GPORT_IS_MPLS_PORT(endpoint_info->gport)) {
            return BCM_E_PARAM;
        }

        /*
         * If ACH is present, UDP/IP and RAW can be used
         * Otherwise, only UDP/IP can be set
         */
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_ACH) {
            packet_flags |= _BCM_BFD_ENCAP_PKT_G_ACH;
            /* Raw or UDP/IP */
            if (!(endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW)) {
                packet_flags |= _BCM_BFD_ENCAP_PKT_G_ACH__IP;
            }
        } else {
            if (endpoint_info->flags & BCM_BFD_ENDPOINT_PWE_RAW) {
                return BCM_E_PARAM;
            }
            packet_flags |= _BCM_BFD_ENCAP_PKT_G_ACH__IP;
        }
        
        if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH__IP) {
           /* UDP/IP */
            _BCM_BFD_UDP_SRC_PORT_VALIDATE(endpoint_info->udp_src_port);
            packet_flags |= _BCM_BFD_ENCAP_PKT_G_ACH__IP;
            packet_flags |=
                (_BCM_BFD_ENCAP_PKT_IP | ip_flag |
                 _BCM_BFD_ENCAP_PKT_UDP);

            BCM_IF_ERROR_RETURN(_bcm_petra_bfd_uc_ip_validate_2(endpoint_info,
                                                            packet_flags));
        }
        break;

    default:
        return BCM_E_PARAM;
    }

    /* Check if HG Header is needed (TX port is HG) */
    SOC_PBMP_PORT_SET(tx_pbmp, tx_port);
    SOC_PBMP_ASSIGN(st_pbmp, PBMP_ST_ALL(unit));
    BCM_API_XLATE_PORT_PBMP_P2A(unit, &st_pbmp);
    SOC_PBMP_AND(st_pbmp, tx_pbmp);
    if (SOC_PBMP_NOT_NULL(st_pbmp)) {
        if (IS_HG2_ENABLED_PORT(unit, tx_port)) {
            packet_flags |= _BCM_BFD_ENCAP_PKT_HG2;
        } else {
            packet_flags |= _BCM_BFD_ENCAP_PKT_HG;
        }
    }

    /* Build header/labels and pack in buffer */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_encap_build_pack(unit,
                                        endpoint_config,
                                        packet_flags,
                                        encap_data));

    /*
     * Set encap type.  Used to indicate uC for special
     * handling on certain encapsulation types:
     * - UDP checksum update is required
     * - packet is MPLS-TP CC only
     * - packet is MPLS-TP CC/CV
     */
    if (packet_flags & _BCM_BFD_ENCAP_PKT_UDP) {
        if (packet_flags & _BCM_BFD_ENCAP_PKT_IP__V6) {
            endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_V6UDP;
        } else {
            endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_V4UDP;
        }
    } else if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH__CC) {
        endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_MPLS_TP_CC;
    } else if (packet_flags & _BCM_BFD_ENCAP_PKT_G_ACH__CC_CV) {
        endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_MPLS_TP_CC_CV;
    } else {
        endpoint_config->encap_type = SHR_BFD_ENCAP_TYPE_RAW;
    }                    

#ifdef BFD_DEBUG_DUMP
    _bcm_petra_bfd_uc_encap_data_dump(encap_data, endpoint_config->encap_length);
#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_petra_bfd_uc_is_init
 * Purpose:
 *      Whether uKernel BFD has been inited.
 * Parameters:
 *      unit       (IN) unit number
 */
int 
bcm_petra_bfd_uc_is_init(int unit) {
    int initialized = 0, rv = 0;
    uint8 is_allocated = 0;

    BCMDNX_INIT_FUNC_DEFS;

    rv = BFDIPV6_ACCESS.is_allocated(unit, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_allocated == 1) {
        if (BFD_DPP_UC_INIT(unit)) {
            initialized = 1;
        }
    }

    return initialized;

    exit:
        BCMDNX_FUNC_RETURN;
}
    
/*
 * Function:
 *      bcm_petra_bfd_uc_init
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
int 
bcm_petra_bfd_uc_init(int unit)
{
    int rv = SOC_E_NONE;
    bfd_sdk_msg_ctrl_init_t msg_init;
    uint8 *buffer, *buffer_ptr;
    uint16 send_len;
    uint16 reply_len;
    uint16 buffer_len;
    int uc = 0;
    int num_sessions;
    int num_auth_sha1, num_auth_sp;
    int cpu_cosq;
    int priority;
    uint32 num_punt_buffers = 0;
    sal_thread_t event_thread_id;
    uint8 is_allocated;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info;
    uint8 bfd_no_down_pkt_on_sess_delete = 0;
    uint8 bfd_not_check_trap_code_on_rx_pkt = 0;
    bfd_sdk_msg_ctrl_trace_log_enable_t msg_trace;
    uint32 protection_packet_enable = 0;
    uint32 protection_pkt_event_fifo_size = 0;
    uint32 protection_pkt_event_fifo_timer = 0;
    uint8 use_localdiscr_as_sess_id = 0;
    uint8 static_remote_disc = 0;

    BCMDNX_INIT_FUNC_DEFS;


    if (bcm_petra_bfd_uc_is_init(unit)) {
#ifdef BCM_WARM_BOOT_SUPPORT
        if (!SOC_WARM_BOOT(unit)) {
#endif /*BCM_WARM_BOOT_SUPPORT*/
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Error: BFD IPV6 already initialized.\n")));
            return BCM_E_NONE;
        }
#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif /*BCM_WARM_BOOT_SUPPORT*/
    }
    if (!SOC_WARM_BOOT(unit)) {
        /* Software state init */
        rv = BFDIPV6_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = BFDIPV6_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    }
    /* 
     * Get SOC properties 
     */
    num_sessions = soc_property_get(unit, spn_BFD_NUM_SESSIONS, 256);
    if (num_sessions > _BCM_BFD_MAX_NUM_SESSIONS) {
        return BCM_E_CONFIG;
    }
    num_auth_sha1 = soc_property_get(unit, spn_BFD_SHA1_KEYS, 0);
    num_auth_sp   = soc_property_get(unit, spn_BFD_SIMPLE_PASSWORD_KEYS, 0);
    cpu_cosq      = soc_property_get(unit, spn_BFD_COSQ, _BCM_BFD_COSQ_INVALID);
    num_punt_buffers = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "num_punt_buffers", 1 /*DEFAULT VALUE*/);
    bfd_no_down_pkt_on_sess_delete = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "no_down_pkt_on_sess_del", 0 /*DEFAULT VALUE*/);
    bfd_not_check_trap_code_on_rx_pkt = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "not_check_trap_code_on_rx_pkt", 0 /*DEFAULT VALUE*/);
    /* BFD Protection Packet Feature */
    protection_packet_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_ipv6_protection", 0 /*DEFAULT VALUE*/);
    protection_pkt_event_fifo_size = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_ipv6_event_fifo_size", 10 /*DEFAULT VALUE*/);
    protection_pkt_event_fifo_timer = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "bfd_ipv6_event_fifo_timer", 3300 /*DEFAULT VALUE*/);
    use_localdiscr_as_sess_id   = soc_property_get(unit,
                              spn_BFD_USE_LOCAL_DISCRIMINATOR_AS_SESSION_ID, 0);

    static_remote_disc   = soc_property_get(unit, spn_BFD_IPV6_STATIC_REMOTE_DISCRIMINATOR, 0);

    /*
     * Initialize HOST side
     */
    rv = _bcm_petra_bfd_uc_alloc_resource(unit, num_sessions, num_auth_sha1,
                                        num_auth_sp, use_localdiscr_as_sess_id);
    if (BCM_FAILURE(rv)) {
        _bcm_petra_bfd_uc_free_resource(unit);
        return rv;
    }

    bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    /*
     * Start BFD application in BTE (Broadcom Task Engine) uController.
     * Determine which uController is running BFD  by choosing the first
     * uC that returns successfully.
     */
    rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_BFD,
                               _BCM_BFD_UC_MSG_TIMEOUT_USECS,
                               BFD_SDK_VERSION,
                               _BCM_BFD_UC_MIN_VERSION,
                               NULL, NULL);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

 
    bfd_ipv6_dpp_info->uc_num = uc;
    if (use_localdiscr_as_sess_id) {
        bfd_ipv6_dpp_info->endpoint_count = _BCM_BFD_MAX_NUM_ENDPOINTS;
        bfd_ipv6_dpp_info->session_count = num_sessions;
    } else {
        bfd_ipv6_dpp_info->endpoint_count = num_sessions;
    }

    bfd_ipv6_dpp_info->num_auth_sha1_keys = num_auth_sha1;
    bfd_ipv6_dpp_info->num_auth_sp_keys   = num_auth_sp;
    bfd_ipv6_dpp_info->cpu_cosq           = cpu_cosq; 
    
    /* RX DMA channel (0..3) local to the uC */
    bfd_ipv6_dpp_info->rx_channel = _BCM_BFD_RX_CHANNEL; 

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
#endif
    /* Set control message data */
    sal_memset(&msg_init, 0, sizeof(msg_init));

    msg_init.encap_size         = 0;    /* Not used */
    msg_init.num_sessions       = num_sessions;
    msg_init.num_auth_sha1_keys = bfd_ipv6_dpp_info->num_auth_sha1_keys;
    msg_init.num_auth_sp_keys   = bfd_ipv6_dpp_info->num_auth_sp_keys;
    msg_init.rx_channel         = bfd_ipv6_dpp_info->rx_channel;
    msg_init.num_punt_buffers   = num_punt_buffers;
    /* Default behavior of C bit set */
    msg_init.config_flags  |=   BFD_CONFIG_CONTROL_PLANE_BIT;
    msg_init.config_flags  |=
            (bfd_no_down_pkt_on_sess_delete?BFD_CONFIG_NO_DOWN_PKT_ON_SESS_DELETE:0);
    msg_init.config_flags  |=
            (bfd_not_check_trap_code_on_rx_pkt?BFD_CONFIG_NOT_CHECK_TRAP_CODE_ON_RX_PKT:0);
    msg_init.config_flags  |=
            (use_localdiscr_as_sess_id?BFD_CONFIG_LOCALDISCR_AS_SESSION_ID:0);
    msg_init.config_flags |=
            (static_remote_disc ? BFD_CONFIG_STATIC_REMOTE_DISCR : 0);

    /* Protection Packet Feature Init */
    msg_init.protection_packet_enable = protection_packet_enable;
    msg_init.protection_pkt_event_fifo_size = protection_pkt_event_fifo_size;
    msg_init.protection_pkt_event_fifo_timer = protection_pkt_event_fifo_timer;

    /* Pack control message data into DMA buffer */
    buffer = bfd_ipv6_dpp_info->dma_buffer;
    buffer_ptr = bfd_sdk_dpp_msg_ctrl_init_pack(buffer, &msg_init);
    send_len = buffer_ptr - buffer;
 
    /* Send BFD Init message to uC */
    rv = _bcm_petra_bfd_uc_msg_send_receive(unit,
                                MOS_MSG_SUBCLASS_BFD_INIT,
                                send_len, 0,
                                MOS_MSG_SUBCLASS_BFD_INIT_REPLY,
                                &reply_len);
    if (BCM_FAILURE(rv) || (reply_len != 0)) {
        _bcm_petra_bfd_uc_free_resource(unit);
        if (BCM_SUCCESS(rv)) {
            rv = BCM_E_INTERNAL;
        }
        return rv;
    }

    bfd_ipv6_dpp_info->trace_addr = 0;
    if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_DBG_TRACE)) {
        BCM_IF_ERROR_RETURN(
           _bcm_petra_bfd_uc_msg_send_receive(unit,
                                              MOS_MSG_SUBCLASS_BFD_TRACE_LOG_ENABLE,
                                              BFD_TRACE_LOG_MAX_SIZE, 0,
                                              MOS_MSG_SUBCLASS_BFD_TRACE_LOG_ENABLE_REPLY,
                                              &reply_len));

        sal_memset(&msg_trace, 0, sizeof(msg_trace));
        buffer     = bfd_ipv6_dpp_info->dma_buffer;
        buffer_ptr = bfd_sdk_dpp_msg_ctrl_trace_log_enable_unpack(buffer, &msg_trace);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            return BCM_E_INTERNAL;
        }
        bfd_ipv6_dpp_info->trace_addr = msg_trace.addr_lo;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif
    /*
     * Initialize Event handler
     */
    /* Start event thread handler */
    priority = soc_property_get(unit, spn_BFD_THREAD_PRI, BCM_BFD_THREAD_PRI_DFLT);
    event_thread_id = bfd_ipv6_dpp_info->event_thread_id;

    if (event_thread_id == NULL) {
        if (sal_thread_create("bcmBFD", SAL_THREAD_STKSZ, 
                              priority,
                              (void (*)(void*))_bcm_petra_bfd_uc_callback_thread,
                              INT_TO_PTR(unit)) == SAL_THREAD_ERROR) {
            if (SAL_BOOT_QUICKTURN) {
                /* If emulation, then wait */
                sal_usleep(1000000);
            }

            event_thread_id = bfd_ipv6_dpp_info->event_thread_id;
            if (event_thread_id == NULL) {
                BCM_IF_ERROR_RETURN(bcm_petra_bfd_uc_deinit(unit));
                return BCM_E_MEMORY;
            }
        }
    }

    bfd_ipv6_dpp_info->event_thread_kill = 0;
    bfd_ipv6_dpp_info->initialized = 1;

    return rv;

    exit:
        BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_bfd_uc_deinit
 * Purpose:
 *      Send a BFD message to deinit the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
int 
bcm_petra_bfd_uc_deinit(int unit)
{
    int rv = SOC_E_NONE;
    uint16 reply_len;
    sal_usecs_t timeout = 0;
    int uc_num;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    if (!bcm_petra_bfd_uc_is_init(unit)) {
        return BCM_E_NONE;
    }


     /* Event Handler thread exit signal */
    if (bfd_ipv6_dpp_info->event_thread_id) {
        bfd_ipv6_dpp_info->event_thread_kill = 1;
        uc_num = bfd_ipv6_dpp_info->uc_num;

        soc_cmic_uc_msg_receive_cancel(unit, uc_num,
                                   MOS_MSG_CLASS_BFD_EVENT);

        /*
         * Ensure here itself that the callback thread exists as we are going
         * to free bcm_xgs5_bfd_info below and we will loose reference
         * to the thread
         */
        timeout = sal_time_usecs() + 5000000;

        while (bfd_ipv6_dpp_info->event_thread_id != NULL) {
            if (sal_time_usecs() < timeout) {
                /*give some time to already running bfd thread to schedule and exit*/
                sal_usleep(1000);
            } else {
                /*timeout*/
                LOG_ERROR(BSL_LS_BCM_BFD,
                          (BSL_META_U(unit,
                                      "BFD event thread did not exit.\n")));
                return BCM_E_INTERNAL;
            }
        }
    }

    if (!SOC_WARM_BOOT(unit)) {
        /*
         * Send BFD Uninit message to uC
         * Ignore error since that may indicate uKernel was reloaded.
         */
        rv = _bcm_petra_bfd_uc_msg_send_receive(unit,
                                MOS_MSG_SUBCLASS_BFD_UNINIT,
                                0, 0,
                                MOS_MSG_SUBCLASS_BFD_UNINIT_REPLY,
                                &reply_len);
        if (BCM_SUCCESS(rv) && (reply_len != 0)) {
            return BCM_E_INTERNAL;
        }
        /*
         * Release memory and set 'bcm_dpp_bfd_ipv6_info[unit]' to NULL.
         * Following this procedure, then, 'bfd_ipv6_dpp_info' can not be used.
         */
        _bcm_petra_bfd_uc_free_resource(unit);
    }
    return rv;

}


/*
 * Function:
 *      bcm_petra_bfd_uc_tx_start
 * Purpose:
 *      Start all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to start TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_tx_start(int unit)
{
    uint16 reply_len;
    int index;
    int ep_found = 0;
    int rv = 0;
    int ep_count = 0;
    uint8 ep_in_use;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);
    ep_count =  bfd_ipv6_dpp_info->endpoint_count;

    for (index = 0; index < ep_count; index++) {
        rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, index, &ep_in_use);
        BCMDNX_IF_ERR_EXIT(rv);
        if (ep_in_use) {
           ep_found = 1;
           break;
        }
    }

    if (!ep_found) {
        return BCM_E_NOT_FOUND;
    }

    /* Send BFD Session Delete All message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive
         (unit,
          MOS_MSG_SUBCLASS_BFD_TX_START,
          0, 0,
          MOS_MSG_SUBCLASS_BFD_TX_START_REPLY,
          &reply_len));

    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_tx_stop
 * Purpose:
 *      Stop all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to stop TX which does not exist
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_tx_stop(int unit)
{
    uint16 reply_len;
    int index;
    int ep_found = 0;
    int rv = 0;
    int ep_count = 0;
    uint8 ep_in_use;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    ep_count = bfd_ipv6_dpp_info->endpoint_count;

    for (index = 0; index < ep_count; index++) {
        rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, index, &ep_in_use);
        BCMDNX_IF_ERR_EXIT(rv);
        if (ep_in_use) {
            ep_found = 1;
            break;
        }
    }

    if (!ep_found) {
        return BCM_E_NOT_FOUND;
    }

    /* Send BFD Session Delete All message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive
         (unit,
          MOS_MSG_SUBCLASS_BFD_TX_STOP,
          0, 0,
          MOS_MSG_SUBCLASS_BFD_TX_STOP_REPLY,
          &reply_len));

    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_find_session_id_from_endpoint_id
 * Purpose:
 *      Finds a session ID associated with an endpoint ID.
 * Parameters:
 *      unit       (IN) unit number
 *      endpoint   (IN) endpoint ID to which session map needed
 *      session_id (OUT) session ID associated with EP id
 */
STATIC int
_bcm_petra_bfd_uc_find_session_id_from_endpoint_id(int unit,
                                             bcm_bfd_endpoint_t endpoint,
                                             bcm_bfd_endpoint_t *session_id)
 
{
    int index;
    int ep_found = 0;
    int rv = 0;
    int num_sessions = 0;
    bcm_bfd_endpoint_t endpoint_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    num_sessions = bfd_ipv6_dpp_info->session_count;

    for (index = 0; index < num_sessions; index++) {
        rv = BFDIPV6_ACCESS.endpoint_to_session_map.get(unit,
                                          index,
                                          &endpoint_id);
        BCMDNX_IF_ERR_EXIT(rv);
        if (endpoint == endpoint_id) {
           ep_found = 1;
           *session_id = index;
           break;
        }
    }

    if (!ep_found) {
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_bfd_uc_endpoint_to_session_id_map_set
 * Purpose:
 *      Allocates and associates an endpoint ID to a session ID.
 * Parameters:
 *      unit       (IN) unit number
 *      endpoint   (IN) endpoint ID to which session map needed
 *      session_id (OUT) session ID generated
 */
STATIC int
_bcm_petra_bfd_uc_endpoint_to_session_id_map_set(int unit,
                                             bcm_bfd_endpoint_t endpoint,
                                             int *session_id)
 
{
    bcm_error_t rv = BCM_E_NONE;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    /* Allocate session ID */
    *session_id =
        _bcm_petra_bfd_uc_find_free_session(unit,
                bfd_ipv6_dpp_info->session_count);
    if (*session_id < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL,
                (_BSL_BCM_MSG("Error: BFD session"
                              " database full.\n")));
    }

    rv = BFDIPV6_ACCESS.endpoint_to_session_map.set(unit,
                                   *session_id, endpoint);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Mark session in use */
    rv = BFDIPV6_ACCESS.sessions_in_use.bit_set(unit, *session_id);
    BCMDNX_IF_ERR_EXIT(rv);

    return rv;

    exit:
        BCMDNX_FUNC_RETURN;

}

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_create
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
int 
bcm_petra_bfd_uc_endpoint_create(int unit,
                                        bcm_bfd_endpoint_info_t *endpoint_info)
{
#ifdef BCM_CMICM_SUPPORT
    bcm_error_t rv = BCM_E_NONE;
    bfd_endpoint_config_t endpoint_config = {0};
    bfd_endpoint_config_t  endpoint_config_cur = {0};
    bcm_bfd_endpoint_info_t temp_info;
    int update=0;
    int endpoint_index;
    bcm_module_t module_id;
    bcm_port_t port_id;
    bcm_port_t tx_port;
    bcm_l3_egress_t l3_egress;
    bfd_sdk_msg_ctrl_sess_set_t msg_sess;
    uint8 *buffer = NULL, *buffer_ptr = NULL;
    uint16 buffer_len, reply_len;
    int encap = 0;
    uint32 session_flags;
    bfd_mpls_label_t mpls_label;    
    uint8 ep_in_use;
    int endpoint_count, session_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info;
    uint8 is_m_hop_extended = 0;
    uint8 is_non_bfd_forwarding = 0;
    uint8 local_echo = 0;
    uint8 use_localdiscr_as_sess_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(endpoint_info);

    sal_memset(&endpoint_config, 0, sizeof(endpoint_config));
    sal_memset(&endpoint_config_cur, 0, sizeof(endpoint_config_cur));

    if (endpoint_info->type == bcmBFDTunnelTypeMplsPhp) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
            (_BSL_BCM_MSG("Error: Tunnel type bcmBFDTunnelTypeMplsPhp is not supported for BFD over IPv6.\n")));
    }
    /* Seamless BFD feature is not supported for BFD over IPv6 endpoints */
    if (_BCM_BFD_ENDPOINT_IS_SBFD(unit, endpoint_info)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
            (_BSL_BCM_MSG("Error: Seamless BFD is not supported for BFD over IPv6 endpoints.\n")));
    }

    /* Enable the BFD echo mode */
    local_echo = (endpoint_info->flags & BCM_BFD_ECHO) ? 1 : 0;

    /* BFD Echo mode should be enabled with IPv6 Enacap only*/
    if (local_echo && (endpoint_info->type != bcmBFDTunnelTypeUdp)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
            (_BSL_BCM_MSG("Error: ECHO mode is supported for BFD over IPv6 tunnel only.\n")));
    }

    if (local_echo && !BFD_UC_FEATURE_CHECK(BFD_FEATURE_ECHO_MODE)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
            (_BSL_BCM_MSG("Error: BFD Echo Mode is not supported in uKernel.\n")));
    }
    
    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) &&
        (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
        is_m_hop_extended = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0);
    }
    if ((endpoint_info->type == bcmBFDTunnelTypeUdp) &&
        !(endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
        is_non_bfd_forwarding = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0));
    }

    /* Micro BFD is a single hop protocol. */
    if (((endpoint_info->flags & BCM_BFD_ENDPOINT_MICRO_BFD) != 0) && 
        ((endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)  != 0)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                    (_BSL_BCM_MSG("Error: BCM_BFD_ENDPOINT_MULTIHOP flag may not be defined with BCM_BFD_ENDPOINT_MICRO_BFD.")));
    }

    /* Check Authentication key index */
    if (endpoint_info->auth == bcmBFDAuthTypeNone) {
        /* Nothing to check */
    } else if (endpoint_info->auth == bcmBFDAuthTypeSimplePassword) {
        AUTH_SP_INDEX_CHECK(unit, endpoint_info->auth_index);
    } else if ((endpoint_info->auth == bcmBFDAuthTypeKeyedSHA1) ||
               (endpoint_info->auth == bcmBFDAuthTypeMeticulousKeyedSHA1)) {
        AUTH_SHA1_INDEX_CHECK(unit, endpoint_info->auth_index);
    } else {
        /*
         * Not supported:
         * bcmBFDAuthTypeKeyedMD5 and bcmBFDAuthTypeMeticulousKeyedMD5
         */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD auth type not support.\n")));
    }

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP) {
        /*  Zero IPv6 address may not be used*/
        uint8 zero_ip6_addr[16] = { 0 };

        if (sal_memcmp(zero_ip6_addr, endpoint_info->src_ip6_addr, 16) == 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD source address may not be zero.\n")));
        }
    }

    /* A unique local discriminator should be nonzero */
    if (!endpoint_info->local_discr) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD local discriminator cannot be zero.\n")));
    }

    /* Local detection multiplier should be set to Default value, if configured value is 0 */
    if (!endpoint_info->local_detect_mult) {
        endpoint_info->local_detect_mult = _BCM_BFD_DEFAULT_DETECT_MULTIPLIER;
        LOG_VERBOSE(BSL_LS_BCM_BFD,
                    (BSL_META_U(unit,
                                "\n BFD Detect Multiplier Set to Default Value %d\n"), 
                                endpoint_info->local_detect_mult));
    }
    
    /* Resolve module and port */
    sal_memset(&l3_egress, 0, sizeof(l3_egress));
    if (BCM_GPORT_INVALID != endpoint_info->gport) {
        if (_SHR_GPORT_IS_LOCAL(endpoint_info->gport)) {
            BCM_IF_ERROR_RETURN(bcm_stk_modid_get(unit, &module_id));
            port_id = _SHR_GPORT_LOCAL_GET(endpoint_info->gport);
        } else if (_SHR_GPORT_IS_MODPORT(endpoint_info->gport)) {
            module_id = _SHR_GPORT_MODPORT_MODID_GET(endpoint_info->gport);
            port_id = _SHR_GPORT_MODPORT_PORT_GET(endpoint_info->gport);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD gport not support.\n")));
        }
    } else if (BCM_SUCCESS
               (bcm_l3_egress_get(unit, endpoint_info->egress_if, &l3_egress))) {
        module_id = l3_egress.module;
        port_id = _SHR_GPORT_MODPORT_PORT_GET(l3_egress.port);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD module_id and port_id can't get.\n")));
    }
    tx_port = port_id;

    /* Create or Update */
    update = (endpoint_info->flags & BCM_BFD_ENDPOINT_UPDATE) ? 1 : 0;
    session_flags = (update) ? 0 : SHR_BFD_SESS_SET_F_CREATE;

    use_localdiscr_as_sess_id   =
                             bfd_ipv6_dpp_info->use_local_discr_as_session_id;

    if (endpoint_info->flags & BCM_BFD_ENDPOINT_WITH_ID) {
        endpoint_index = endpoint_info->id;

        ENDPOINT_INDEX_CHECK(unit, endpoint_index);

        /*
         * Update requires endpoint ID to exist
         * Create requires endpoint ID not to exist
         */
        rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, endpoint_index, &ep_in_use);
        BCMDNX_IF_ERR_EXIT(rv);
        if (update && !ep_in_use) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: BFD endpoint not fould.\n")));
        } else if (!update &&
                   (ep_in_use)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Error: BFD endpoint alreay exists.\n")));
        }
        
        if (use_localdiscr_as_sess_id) {
            if (update) {
                rv = _bcm_petra_bfd_uc_find_session_id_from_endpoint_id(unit,
                        endpoint_index,
                        &session_id);
                BCMDNX_IF_ERR_EXIT(rv);
                endpoint_index = session_id;
            } else {
                rv = _bcm_petra_bfd_uc_endpoint_to_session_id_map_set(unit,
                                          endpoint_index, &session_id);
                BCMDNX_IF_ERR_EXIT(rv);
                endpoint_index = session_id;
            }
        }

    } else {    /* No ID */

        if (update) {    /* Update specified with no ID */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD endpoint update with no specific ID.\n")));
        }
        endpoint_count = bfd_ipv6_dpp_info->endpoint_count;
        /* Allocate endpoint */
        endpoint_index =
            _bcm_petra_bfd_uc_find_free_endpoint(unit, endpoint_count);
        if (endpoint_index < 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Error: BFD endpoint database full.\n")));
        }

        endpoint_info->id = endpoint_index;

        if (use_localdiscr_as_sess_id) {
            rv = _bcm_petra_bfd_uc_endpoint_to_session_id_map_set(unit,
                                          endpoint_index, &session_id);
            BCMDNX_IF_ERR_EXIT(rv);
            endpoint_index = session_id;
        }
    }    


    if (update && (endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW)) {
        BFD_ENDPOINT_CONFIG(unit, endpoint_index, &endpoint_config);
        rv = bcm_petra_bfd_uc_endpoint_get(unit, endpoint_info->id, &temp_info);
        BCMDNX_IF_ERR_EXIT(rv);
        bcm_petra_memcpy_to_bfd_endpoint_info(&endpoint_config.info, &temp_info);
    }

    memcpy(&endpoint_config_cur, &endpoint_config, sizeof(bfd_endpoint_config_t));

    /* Endpoint tunnel type cannot change */
    if (update && (endpoint_config_cur.info.type != endpoint_info->type)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD endpoint update can't change the type.\n")));
    }

    if (update && (endpoint_config_cur.local_echo !=local_echo)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD endpoint update can't change echo mode.\n")));
    }

    /*
     * Compare new and old BFD parameters for Update
     */
    if (update) {
        if (endpoint_config_cur.info.local_discr !=
            endpoint_info->local_discr) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DISC;
        }
        if (endpoint_config_cur.info.remote_discr !=
            endpoint_info->remote_discr) {
            session_flags |= SHR_BFD_SESS_SET_F_REMOTE_DISC;
        }
        if (endpoint_config_cur.info.local_min_tx !=
            endpoint_info->local_min_tx) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_MIN_TX;
        }
        if (endpoint_config_cur.info.local_min_rx !=
            endpoint_info->local_min_rx) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_MIN_RX;
        }
        if (endpoint_config_cur.info.local_min_echo !=
            endpoint_info->local_min_echo) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_MIN_ECHO_RX;
        }
        if (endpoint_config_cur.info.local_diag !=
            endpoint_info->local_diag) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DIAG;
        }
        if ((endpoint_config_cur.info.flags & BCM_BFD_ENDPOINT_DEMAND)
            != (endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND)) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DEMAND;
        }
        if (endpoint_config_cur.info.local_detect_mult !=
            endpoint_info->local_detect_mult) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_DETECT_MULT;
        }
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_ENCAP_SET) {
            session_flags |= SHR_BFD_SESS_SET_F_ENCAP;
        }
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_SHA1_SEQUENCE_INCR) {
            session_flags |= SHR_BFD_SESS_SET_F_SHA1_XMT_SEQ_INCR;
        }
        if (endpoint_config_cur.info.sampling_ratio !=
            endpoint_info->sampling_ratio) {
            session_flags |= SHR_BFD_SESS_SET_F_SAMPLING_RATIO;
        }
        if (endpoint_info->flags & BCM_BFD_ENDPOINT_STANDBY_SSO_UPDATE) {
            session_flags |= SHR_BFD_SESS_SET_F_SSO_UPDATE;
        }
        if (local_echo != endpoint_config_cur.local_echo) {
            session_flags |= SHR_BFD_SESS_SET_F_LOCAL_ECHO;
        }
    }

    /* Can be set both during endpoint create and update */
    if (endpoint_info->flags & BCM_BFD_ENDPOINT_ADMIN_DOWN_PKT_ON_ENDPOINT_DEL) {
        session_flags |= SHR_BFD_SESS_SET_F_ADMIN_DOWN_PKT_ON_SESS_DEL;
    }

    /* Set Endpoint config entry */
    endpoint_config_cur.endpoint_index = endpoint_index;
    endpoint_config_cur.modid          = module_id;
    endpoint_config_cur.port           = port_id;
    endpoint_config_cur.tx_port        = tx_port;
    endpoint_config_cur.local_echo     = local_echo;
    bcm_petra_memcpy_to_bfd_endpoint_info(&endpoint_config_cur.info, endpoint_info);

    /* Set BFD Session in HW for single-hop classifier*/
    if (!update || (endpoint_info->flags & BCM_BFD_ENDPOINT_ENCAP_SET)) {
        uint32 trap_code = 0;
        uint8 fwd_strength = 0;
        uint32 snp_strength = 0;
    
        if (endpoint_info->remote_gport != BCM_GPORT_INVALID) {
            BCMDNX_IF_ERR_EXIT
                (_bcm_petra_bfd_uc_trap_code_create(unit, endpoint_info, &trap_code));
            fwd_strength = BCM_GPORT_TRAP_GET_STRENGTH(endpoint_info->remote_gport);
        }        
        if (((SOC_IS_JERICHO(unit) && 
            ((SOC_DPP_CONFIG(unit)->pp.micro_bfd_support != SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv6) && 
            (SOC_DPP_CONFIG(unit)->pp.micro_bfd_support != SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4_6)))
            || (endpoint_info->flags & BCM_BFD_ENDPOINT_MULTIHOP)) && !is_m_hop_extended && !is_non_bfd_forwarding) {
            /* Use the OAM classifier*/
            BCMDNX_IF_ERR_EXIT
                (_bcm_petra_bfd_uc_m_hop_set_classifer(unit, endpoint_info, trap_code, fwd_strength, &endpoint_config_cur));
        } else {  
            ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO lem_info;
            ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO_clear(&lem_info);

            fwd_strength = fwd_strength?fwd_strength:_ARAD_PP_OAM_TRAP_STRENGTH;
            snp_strength = 0;                
            rv = _bcm_bfd_endpoint_info_to_lem_info(unit,endpoint_info, &lem_info, trap_code, fwd_strength, snp_strength);
            BCMDNX_IF_ERR_EXIT(rv);

            rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lem_access_bfd_one_hop_lem_entry_add, (unit,&lem_info));
            BCMDNX_IF_ERR_EXIT(rv);           
        }
        encap = 1;
    }

    /* Set BFD Session in HW for single-hop processer when accelerate mode and remote destination is unset*/
    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) && 
        (endpoint_info->remote_gport == BCM_GPORT_INVALID)) {
        /* Set control message data */
        sal_memset(&msg_sess, 0, sizeof(msg_sess));

        /*
         * Set the BFD encapsulation data
         *
         * The function bcm_petra_bfd_uc_encap_create() is called first
         * since this sets some fields in 'endpoint_config' which are
         * used in the message.
         */
        if (encap) {
            BCMDNX_IF_ERR_EXIT
                (_bcm_petra_bfd_uc_encap_create(unit, &endpoint_config_cur,
                                            msg_sess.encap_data));
        }

        msg_sess.sess_id = endpoint_config_cur.endpoint_index;
        msg_sess.flags   = session_flags;
        msg_sess.passive =
            (endpoint_info->flags & BCM_BFD_ENDPOINT_PASSIVE) ? 1 : 0;
        msg_sess.local_demand =
            (endpoint_info->flags & BCM_BFD_ENDPOINT_DEMAND) ? 1 : 0;
        msg_sess.local_diag           = endpoint_info->local_diag;
        msg_sess.local_detect_mult    = endpoint_info->local_detect_mult;
        msg_sess.local_discriminator  = endpoint_info->local_discr;
        msg_sess.remote_discriminator = endpoint_info->remote_discr;
        msg_sess.local_min_tx         = endpoint_info->local_min_tx;
        msg_sess.local_min_rx         = endpoint_info->local_min_rx;
        msg_sess.local_min_echo_rx    = endpoint_info->local_min_echo;
        msg_sess.auth_type            = endpoint_info->auth;
        msg_sess.auth_key             = endpoint_info->auth_index;
        msg_sess.xmt_auth_seq         = endpoint_info->tx_auth_seq;
        if (encap) {
            msg_sess.encap_type   = endpoint_config_cur.encap_type;
            msg_sess.encap_length = endpoint_config_cur.encap_length;
            msg_sess.lkey_etype   = endpoint_config_cur.lkey_etype;
            msg_sess.lkey_offset  = endpoint_config_cur.lkey_offset;
            msg_sess.lkey_length  = endpoint_config_cur.lkey_length;
            
            if (endpoint_info->mep_id_length >
                BCM_BFD_ENDPOINT_MAX_MEP_ID_LENGTH) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Error: BFD endpoint mep id length to long.\n")));
            }

            if (endpoint_info->mep_id_length) {
                msg_sess.mep_id_length = endpoint_info->mep_id_length;
                sal_memcpy(msg_sess.mep_id, endpoint_info->mep_id,
                           endpoint_info->mep_id_length);
            }
        }
        /* MPLS label needs to be in packet frame format (label field only) */
        _bcm_petra_bfd_uc_mpls_label_get(endpoint_info->label, 0, 0, 0,
                                     &mpls_label);
        _bcm_petra_bfd_uc_mpls_label_pack(msg_sess.mpls_label, &mpls_label);

        msg_sess.tx_port = endpoint_config_cur.tx_port;
        msg_sess.tx_cos  = endpoint_info->int_pri;
        msg_sess.tx_pri  = 0;
        msg_sess.tx_qnum = SOC_INFO(unit).port_uc_cosq_base[msg_sess.tx_port] +
            endpoint_info->int_pri;

        if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_PUNT_MODE)) {
            msg_sess.sampling_ratio = endpoint_info->sampling_ratio;
        }

        if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_SSO_UPDATE)) {
            msg_sess.actual_tx_interval = endpoint_info->bfd_period;
            msg_sess.remote_detect_mult = endpoint_info->remote_detect_mult;
        }
        msg_sess.local_echo           = local_echo;
        /* Pack control message data into DMA buffer */
        buffer = bfd_ipv6_dpp_info->dma_buffer;

        buffer_ptr = bfd_sdk_dpp_msg_ctrl_sess_set_pack(buffer, &msg_sess);
        buffer_len = buffer_ptr - buffer;

        /* Send BFD Session Set message to uC */
        BCMDNX_IF_ERR_EXIT
            (_bcm_petra_bfd_uc_msg_send_receive(unit,
                                            MOS_MSG_SUBCLASS_BFD_SESS_SET,
                                            buffer_len, 0,
                                            MOS_MSG_SUBCLASS_BFD_SESS_SET_REPLY,
                                            &reply_len));
        if (reply_len != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: BFD session set message failure.\n")));
        }
    }

    memcpy(&endpoint_config, &endpoint_config_cur, sizeof(bfd_endpoint_config_t));

    rv = BFDIPV6_ACCESS.endpoints.set(unit, endpoint_config.endpoint_index, &endpoint_config);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Mark endpoint in use */
    rv = BFDIPV6_ACCESS.endpoints_in_use.bit_set(unit, endpoint_info->id);
    BCMDNX_IF_ERR_EXIT(rv);

    exit:
        if ((use_localdiscr_as_sess_id) && (!update) &&
            ((rv != BCM_E_NONE) || (_rv != BCM_E_NONE))) {
            bcm_error_t rc = BCM_E_NONE;
            uint8 session_in_use = 0;
            rc = BFDIPV6_ACCESS.sessions_in_use.bit_get(unit,
                                                        session_id,
                                                        &session_in_use);
            if ((rc == BCM_E_NONE) && (session_in_use)) {
                rc = BFDIPV6_ACCESS.sessions_in_use.bit_clear(unit, session_id);
            }
            if (rc != BCM_E_NONE) {
                _rv = rc;
            }
        }
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}






/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_get
 * Purpose:
 *      Send a BFD message to init the BFD function.
 * Parameters:
 *      unit       (IN) unit number
 */
int 
bcm_petra_bfd_uc_endpoint_get(int unit,
                                    bcm_bfd_endpoint_t endpoint,
                                    bcm_bfd_endpoint_info_t *endpoint_info)
{
#ifdef BCM_CMICM_SUPPORT
    bfd_endpoint_config_t endpoint_config;
    bfd_sdk_msg_ctrl_sess_get_t msg_sess;
    uint8 *buffer = NULL, *buffer_ptr = NULL;
    uint16 buffer_len, reply_len;
    uint8 ep_in_use;
    int rv = 0, session_id = 0;
    uint8 use_localdiscr_as_sess_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(endpoint_info);

    ENDPOINT_INDEX_CHECK(unit, endpoint);

    rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, endpoint, &ep_in_use);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!ep_in_use) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: BFD session not found.\n")));
    }

    use_localdiscr_as_sess_id   =
                             bfd_ipv6_dpp_info->use_local_discr_as_session_id;
    if (use_localdiscr_as_sess_id) {
        rv = _bcm_petra_bfd_uc_find_session_id_from_endpoint_id(unit,
                endpoint,
                &session_id);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint = session_id;
    }
    BFD_ENDPOINT_CONFIG(unit, endpoint, &endpoint_config);

    bcm_petra_memcpy_to_bcm_bfd_endpoint_info(endpoint_info, &endpoint_config.info);

    if ((endpoint_info->flags & BCM_BFD_ENDPOINT_IN_HW) && 
        (endpoint_info->remote_gport == BCM_GPORT_INVALID)) {
        /* Send BFD Session Get message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_msg_send_receive(unit,
                                            MOS_MSG_SUBCLASS_BFD_SESS_GET,
                                            endpoint, 0,
                                            MOS_MSG_SUBCLASS_BFD_SESS_GET_REPLY,
                                            &reply_len));

        /* Unpack control message data from DMA buffer */
        sal_memset(&msg_sess, 0, sizeof(msg_sess));

        buffer = bfd_ipv6_dpp_info->dma_buffer;
        buffer_ptr = bfd_sdk_dpp_msg_ctrl_sess_get_unpack(buffer, &msg_sess);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: BFD session get message failure.\n")));
        }
        
        endpoint_info->local_state        = msg_sess.local_sess_state;
        endpoint_info->local_discr        = msg_sess.local_discriminator;
        endpoint_info->local_diag         = msg_sess.local_diag;
        endpoint_info->tx_auth_seq        = msg_sess.xmt_auth_seq;
        endpoint_info->rx_auth_seq        = msg_sess.rcv_auth_seq;
        endpoint_info->remote_state       = msg_sess.remote_sess_state;
        endpoint_info->remote_discr       = msg_sess.remote_discriminator;
        endpoint_info->remote_diag        = msg_sess.remote_diag;
        endpoint_info->remote_min_tx      = msg_sess.remote_min_tx;
        endpoint_info->remote_min_rx      = msg_sess.remote_min_rx;
        endpoint_info->remote_min_echo    = msg_sess.remote_min_echo_rx;
        endpoint_info->remote_detect_mult = msg_sess.remote_detect_mult;

        endpoint_info->remote_flags = 0;
        if (msg_sess.remote_demand) {
            endpoint_info->remote_flags |= BCM_BFD_ENDPOINT_REMOTE_DEMAND;
        }

        if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_POLL_BIT)) {
            endpoint_info->local_flags = 0;
            if (msg_sess.poll) {
                endpoint_info->local_flags |= 1 << 5;
            }
        }

        if (BFD_UC_FEATURE_CHECK(BFD_FEATURE_PUNT_MODE)) {
            endpoint_info->sampling_ratio = msg_sess.sampling_ratio;
        }

        if(BFD_UC_FEATURE_CHECK(BFD_FEATURE_SSO_UPDATE)) {
            endpoint_info->bfd_period = msg_sess.actual_tx_interval;
        }
        /* Get echo mode status from ukernel */
        if (msg_sess.local_echo) {
            endpoint_info->flags |= BCM_BFD_ECHO;
        }

    }
    
    BCM_EXIT;
    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_destroy
 * Purpose:
 *      Destroy an BFD endpoint object.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to destroy.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND Attempt to destroy endpoint which does not exist
 *      BCM_E_INTERNAL  Unable to release resource /
 *                      Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_endpoint_destroy(int unit, bcm_bfd_endpoint_t endpoint)
{
#ifdef BCM_CMICM_SUPPORT
    bcm_error_t rv = BCM_E_NONE;
    bfd_endpoint_config_t endpoint_config;
    uint16 reply_len;
    uint8 ep_in_use;
    uint8 is_m_hop_extended = 0;
    uint8 is_non_bfd_forwarding = 0;
    int session_id = 0, endpoint_id = 0;
    uint8 use_localdiscr_as_sess_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);
    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    ENDPOINT_INDEX_CHECK(unit, endpoint);
    rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, endpoint, &ep_in_use);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!ep_in_use) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Error: BFD session not found.\n")));
    }

    endpoint_id = endpoint;

    use_localdiscr_as_sess_id   =
                             bfd_ipv6_dpp_info->use_local_discr_as_session_id;
    if (use_localdiscr_as_sess_id) {
        rv = _bcm_petra_bfd_uc_find_session_id_from_endpoint_id(unit,
                endpoint,
                &session_id);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint_id = session_id;
    }
    BFD_ENDPOINT_CONFIG(unit, endpoint_id, &endpoint_config);

    if ((endpoint_config.info.type == bcmBFDTunnelTypeUdp) &&
        (endpoint_config.info.flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
        is_m_hop_extended = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0);
    }
    if ((endpoint_config.info.type == bcmBFDTunnelTypeUdp) &&
       !(endpoint_config.info.flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
        is_non_bfd_forwarding = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0));
    }

    if (((SOC_IS_JERICHO(unit) &&
        ((SOC_DPP_CONFIG(unit)->pp.micro_bfd_support != SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv6) && 
        (SOC_DPP_CONFIG(unit)->pp.micro_bfd_support != SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4_6)))
        || (endpoint_config.info.flags & BCM_BFD_ENDPOINT_MULTIHOP)) && !is_m_hop_extended && !is_non_bfd_forwarding) {
        /* Use the OAM classifier*/
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_m_hop_remove_classifer_entry(unit, &endpoint_config));
    }else {
        /* Delete BFD Session in HW for single-hop*/
        rv= MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lem_access_bfd_one_hop_lem_entry_remove, (unit, endpoint_config.info.local_discr, 0));
    	BCM_IF_ERROR_RETURN(rv);
    }

    if ((endpoint_config.info.flags & BCM_BFD_ENDPOINT_IN_HW) &&
        (endpoint_config.info.remote_gport == BCM_GPORT_INVALID)) {
        /* Send BFD Session Delete message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_msg_send_receive(unit,
                                            MOS_MSG_SUBCLASS_BFD_SESS_DELETE,
                                            endpoint_config.endpoint_index, 0,
                                            MOS_MSG_SUBCLASS_BFD_SESS_DELETE_REPLY,
                                            &reply_len));

        if (reply_len != 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: BFD session delete message failure.\n")));
        }
    }

    rv = BFDIPV6_ACCESS.endpoints_in_use.bit_clear(unit, endpoint);
    BCMDNX_IF_ERR_EXIT(rv);

    if (use_localdiscr_as_sess_id) {
        rv = BFDIPV6_ACCESS.sessions_in_use.bit_clear(unit, session_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_destroy_all
 * Purpose:
 *      Destroy all BFD endpoint objects.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_INTERNAL Unable to release resource /
 *                     Failed to read memory or read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_endpoint_destroy_all(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    bcm_error_t rv = BCM_E_NONE;
    bfd_endpoint_config_t endpoint_config;
    uint16 reply_len;
    int index, endpoint = 0;
    int ep_count, session_id = 0;
    uint8 ep_in_use;
    uint8 is_m_hop_extended = 0;
    uint8 is_non_bfd_forwarding = 0;
    uint8 use_localdiscr_as_sess_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);
    ep_count = bfd_ipv6_dpp_info->endpoint_count;

    use_localdiscr_as_sess_id   =
                             bfd_ipv6_dpp_info->use_local_discr_as_session_id;
    for (index = 0; index < ep_count; index++) {
        rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, index, &ep_in_use);
        BCMDNX_IF_ERR_EXIT(rv);

        if (ep_in_use) {
            endpoint = index;
            if (use_localdiscr_as_sess_id) {
                rv = _bcm_petra_bfd_uc_find_session_id_from_endpoint_id(unit,
                        index,
                        &session_id);
                BCMDNX_IF_ERR_EXIT(rv);
                 endpoint = session_id;
            }
            BFD_ENDPOINT_CONFIG(unit, endpoint, &endpoint_config);
            is_m_hop_extended = 0;
            is_non_bfd_forwarding = 0;
            if ((endpoint_config.info.type == bcmBFDTunnelTypeUdp) &&
                    (endpoint_config.info.flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
                is_m_hop_extended = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0);
            }
            if ((endpoint_config.info.type == bcmBFDTunnelTypeUdp) &&
                    !(endpoint_config.info.flags & BCM_BFD_ENDPOINT_MULTIHOP)) {
                is_non_bfd_forwarding = (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0));
            }
            if (((SOC_IS_JERICHO(unit) &&
                            ((SOC_DPP_CONFIG(unit)->pp.micro_bfd_support != SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv6) && 
                             (SOC_DPP_CONFIG(unit)->pp.micro_bfd_support != SOC_DPP_ARAD_MICRO_BFD_SUPPORT_IPv4_6)))
                        || (endpoint_config.info.flags & BCM_BFD_ENDPOINT_MULTIHOP))&& !is_m_hop_extended && !is_non_bfd_forwarding) {
                /* Use the OAM classifier*/
                BCM_IF_ERROR_RETURN
                    (_bcm_petra_bfd_uc_m_hop_remove_classifer_entry(unit, &endpoint_config));
            }else {
                /* Delete BFD Session in HW for single-hop*/
                rv= MBCM_PP_DRIVER_CALL(unit, mbcm_pp_lem_access_bfd_one_hop_lem_entry_remove, (unit, endpoint_config.info.local_discr, 0));
                BCM_IF_ERROR_RETURN(rv);
            }

            rv = BFDIPV6_ACCESS.endpoints_in_use.bit_clear(unit, index);
            BCMDNX_IF_ERR_EXIT(rv);

            if (use_localdiscr_as_sess_id) {
                rv = BFDIPV6_ACCESS.sessions_in_use.bit_clear(unit, session_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
            endpoint_config.endpoint_index = -1;
        }
    }

    /* Send BFD Session Delete All message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive
         (unit,
          MOS_MSG_SUBCLASS_BFD_SESS_DELETE_ALL,
          0, 0,
          MOS_MSG_SUBCLASS_BFD_SESS_DELETE_ALL_REPLY,
          &reply_len));

    if (reply_len != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: BFD session delete all message failure.\n")));
    }

    BCM_EXIT;
    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_poll
 * Purpose:
 *      Poll an BFD endpoint object.  Valid only for endpoint in Demand
 *      Mode and in state bcmBFDStateUp.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll.
 * Returns:
 *      BCM_E_NONE      Operation completed successfully
 *      BCM_E_NOT_FOUND BFD Session ID not in use
 *      BCM_E_INTERNAL  Unable to read or write resource
 *      BCM_E_PARAM     Session not in Demand Mode/
 *                      Session not in UP state
 * Notes: The poll operation will be initiated.  Poll failure will be
 * signalled via asynchronous callback (aka session failure)
 */
int
bcm_petra_bfd_uc_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint)
{
#ifdef BCM_CMICM_SUPPORT
    bfd_endpoint_config_t endpoint_config;
    uint16 reply_len;
    int rv = 0, session_id = 0;
    uint8 ep_in_use;
    uint8 use_localdiscr_as_sess_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);
    BCMDNX_INIT_FUNC_DEFS;
    
    BFD_UC_FEATURE_INIT_CHECK(unit);

    ENDPOINT_INDEX_CHECK(unit, endpoint);

    rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit,endpoint, &ep_in_use);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!ep_in_use) {
        return BCM_E_NOT_FOUND;
    }

    use_localdiscr_as_sess_id   =
                             bfd_ipv6_dpp_info->use_local_discr_as_session_id;
    if (use_localdiscr_as_sess_id) {
        rv = _bcm_petra_bfd_uc_find_session_id_from_endpoint_id(unit,
                endpoint,
                &session_id);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint = session_id;
    }
    BFD_ENDPOINT_CONFIG(unit, endpoint, &endpoint_config);

    /* Check for Demand mode */
    if (!(endpoint_config.info.flags & BCM_BFD_ENDPOINT_DEMAND)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Error: BFD session poll only support in demand mode.\n")));
    }

    /* Issue poll request on session not in the UP state will fail */

    /* Send BFD Session Poll message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive(unit,
                                        MOS_MSG_SUBCLASS_BFD_SESS_POLL,
                                        endpoint_config.endpoint_index, 0,
                                        MOS_MSG_SUBCLASS_BFD_SESS_POLL_REPLY,
                                        &reply_len));

    if (reply_len != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Error: BFD session poll message failure.\n")));
    }

    BCM_EXIT;
    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_event_register
 * Purpose:
 *      Register a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback will be invoked.
 *      cb          - (IN) A pointer to the callback function.
 *      user_data   - (IN) Pointer to user data to pass to the callback.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_petra_bfd_uc_event_register(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data)
{
#ifdef BCM_CMICM_SUPPORT
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);
    uint32 result;
    bfd_event_handler_t *event_handler;
    bfd_event_handler_t *previous = NULL;
    bcm_bfd_event_type_t event_type;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(cb);

    SHR_BITTEST_RANGE(event_types.w, 0, bcmBFDEventCount, result);

    if (result == 0) {
        return BCM_E_PARAM;
    }

    for (event_handler = bfd_ipv6_dpp_info->event_handler_list;
         event_handler != NULL;
         event_handler = event_handler->next) {
        if (event_handler->cb == cb) {
            break;
        }

        previous = event_handler;
    }

    if (event_handler == NULL) {
        /* This handler hasn't been registered yet */

        event_handler = sal_alloc(sizeof(*event_handler), "BFD event handler");

        if (event_handler == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memset(event_handler, 0, sizeof(*event_handler));

        event_handler->next = NULL;
        event_handler->cb = cb;

        SHR_BITCLR_RANGE(event_handler->event_types.w, 0, bcmBFDEventCount);

        if (previous != NULL) {
            previous->next = event_handler;
        } else {
            bfd_ipv6_dpp_info->event_handler_list = event_handler;
        }
    }

    for (event_type = 0; event_type < bcmBFDEventCount; ++event_type) {
        if (SHR_BITGET(event_types.w, event_type)) {
            if (!SHR_BITGET(event_handler->event_types.w, event_type)) {
                /* This handler isn't handling this event yet */
                SHR_BITSET(event_handler->event_types.w, event_type);
            }
        }
    }

    event_handler->user_data = user_data;

    /* Update Events mask */
    return _bcm_petra_bfd_uc_event_mask_set(unit);

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be invoked.
 *      cb          - (IN) A pointer to the callback function.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 * Notes:
 */
int
bcm_petra_bfd_uc_event_unregister(int unit,
                               bcm_bfd_event_types_t event_types,
                               bcm_bfd_event_cb cb)
{
#ifdef BCM_CMICM_SUPPORT
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);
    uint32 result;
    bfd_event_handler_t *event_handler;
    bfd_event_handler_t *previous = NULL;
    bcm_bfd_event_type_t event_type;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(cb);

    SHR_BITTEST_RANGE(event_types.w, 0, bcmBFDEventCount, result);

    if (result == 0) {
        return BCM_E_PARAM;
    }

    for (event_handler = bfd_ipv6_dpp_info->event_handler_list ;
         event_handler != NULL;
         event_handler = event_handler->next) {
        if (event_handler->cb == cb) {
            break;
        }

        previous = event_handler;
    }

    if (event_handler == NULL) {
        return BCM_E_NOT_FOUND;
    }

    for (event_type = 0; event_type < bcmBFDEventCount; ++event_type) {
        if (SHR_BITGET(event_types.w, event_type)) {
            SHR_BITCLR(event_handler->event_types.w, event_type);
        }
    }

    SHR_BITTEST_RANGE(event_handler->event_types.w, 0, bcmBFDEventCount,
        result);

    if (result == 0) {
        /* No more events for this handler to handle */
        if (previous != NULL) {
            previous->next = event_handler->next;
        } else {
            bfd_ipv6_dpp_info->event_handler_list = event_handler->next;
        }

        sal_free(event_handler);
    }

    /* Update Events mask */
    return _bcm_petra_bfd_uc_event_mask_set(unit);

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint statistics.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for.
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to receive
 *                 the data.
 *      clear    - (IN) If set, clear stats after read.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to counter information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_endpoint_stat_get(int unit,
                                bcm_bfd_endpoint_t endpoint,
                                bcm_bfd_endpoint_stat_t *ctr_info, uint8 clear)
{
#ifdef BCM_CMICM_SUPPORT

    shr_bfd_msg_ctrl_stat_req_t stat_req;
    bfd_sdk_msg_ctrl_stat_reply_t stat_reply;
    uint8 *buffer_req = NULL, *buffer_reply = NULL, *buffer_ptr = NULL;
    uint16 buffer_len, reply_len;
    int rv = 0, session_id = 0;
    uint8 ep_in_use;
    uint8 use_localdiscr_as_sess_id = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(ctr_info);

    ENDPOINT_INDEX_CHECK(unit, endpoint);

    rv = BFDIPV6_ACCESS.endpoints_in_use.bit_get(unit, endpoint,  &ep_in_use);
    BCMDNX_IF_ERR_EXIT(rv);

    if(!ep_in_use) {
        return BCM_E_NOT_FOUND;
    }

    use_localdiscr_as_sess_id   =
                             bfd_ipv6_dpp_info->use_local_discr_as_session_id;
    if (use_localdiscr_as_sess_id) {
        rv = _bcm_petra_bfd_uc_find_session_id_from_endpoint_id(unit,
                endpoint,
                &session_id);
        BCMDNX_IF_ERR_EXIT(rv);
        endpoint = session_id;
    }
    /* Set control message data */
    sal_memset(&stat_req, 0, sizeof(stat_req));
    stat_req.sess_id = endpoint;
    stat_req.clear   = clear;

    /* Pack control message data into DMA */
    buffer_req = bfd_ipv6_dpp_info->dma_buffer;


    buffer_ptr = bfd_sdk_dpp_msg_ctrl_stat_req_pack(buffer_req, &stat_req);
    buffer_len = buffer_ptr - buffer_req;

    /* Send BFD Stat Get message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive(unit,
                                        MOS_MSG_SUBCLASS_BFD_STAT_GET,
                                        buffer_len, 0,
                                        MOS_MSG_SUBCLASS_BFD_STAT_GET_REPLY,
                                        &reply_len));

    /* Unpack control message data from DMA buffer */
    sal_memset(&stat_reply, 0, sizeof(stat_reply));
    buffer_reply = bfd_ipv6_dpp_info->dma_buffer;

    buffer_ptr = bfd_sdk_dpp_msg_ctrl_stat_reply_unpack(buffer_reply,
                                                        &stat_reply);
    buffer_len = buffer_ptr - buffer_reply;
    if (reply_len != buffer_len) {
        return BCM_E_INTERNAL;
    }

    ctr_info->packets_in = stat_reply.packets_in;
    ctr_info->packets_out = stat_reply.packets_out;
    ctr_info->packets_drop = stat_reply.packets_drop;
    ctr_info->packets_auth_drop = stat_reply.packets_auth_drop;
    COMPILER_64_SET(ctr_info->packets_echo_reply,
                    stat_reply.packets_echo_reply_hi,
                    stat_reply.packets_echo_reply);

    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure.
 *      sha1  - (IN) Pointer to SHA1 info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_auth_sha1_set(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
#ifdef BCM_CMICM_SUPPORT

    shr_bfd_msg_ctrl_auth_sha1_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int rv = BCM_E_NONE;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(sha1);

    AUTH_SHA1_INDEX_CHECK(unit, index);

    /* Set control message data */
    sal_memset(&msg_auth, 0, sizeof(msg_auth));
    msg_auth.index    = index;
    msg_auth.enable   = sha1->enable;
    msg_auth.sequence = sha1->sequence;
    sal_memcpy(msg_auth.key, sha1->key, _SHR_BFD_AUTH_SHA1_KEY_LENGTH);

    /* Pack control message data into DMA buffer */
    buffer = bfd_ipv6_dpp_info->dma_buffer;
    buffer_ptr = bfd_sdk_dpp_msg_ctrl_auth_sha1_pack(buffer, &msg_auth);
    buffer_len = buffer_ptr - buffer;

    /* Send BFD Auth Sha1 Set message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive
         (unit,
          MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET,
          buffer_len, 0,
          MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_SET_REPLY,
          &reply_len));
    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }
    rv = BFDIPV6_ACCESS.auth_sha1.set(unit, index, sha1);
    BCMDNX_IF_ERR_EXIT(rv);

    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}


/*
 * Function:
 *      bcm_petra_bfd_uc_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve.
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_auth_sha1_get(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
#ifdef BCM_CMICM_SUPPORT

    shr_bfd_msg_ctrl_auth_sha1_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int i, rv = BCM_E_NONE;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(sha1);

    AUTH_SHA1_INDEX_CHECK(unit, index);

    /* If warmboot, get authentication data from uController, else use cache */
    if (SOC_WARM_BOOT(unit)) {

        /* Send BFD Auth Sha1 Get message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_msg_send_receive
             (unit,
              MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_GET,
              index, 0,
              MOS_MSG_SUBCLASS_BFD_AUTH_SHA1_GET_REPLY,
              &reply_len));

        /* Unpack control message data from DMA buffer */
        sal_memset(&msg_auth, 0, sizeof(msg_auth));
        buffer = bfd_ipv6_dpp_info->dma_buffer;
        buffer_ptr = bfd_sdk_dpp_msg_ctrl_auth_sha1_unpack(buffer, &msg_auth);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            return BCM_E_INTERNAL;
        }
        rv = BFDIPV6_ACCESS.auth_sha1.enable.set(unit, index, msg_auth.enable);
        BCMDNX_IF_ERR_EXIT(rv);
        rv = BFDIPV6_ACCESS.auth_sha1.sequence.set(unit, index, msg_auth.sequence);
        BCMDNX_IF_ERR_EXIT(rv);
        for(i = 0; i< _SHR_BFD_AUTH_SHA1_KEY_LENGTH; i++) {
            rv = BFDIPV6_ACCESS.auth_sha1.key.set(unit, index, i, msg_auth.key[i]);
            BCMDNX_IF_ERR_EXIT(rv);
        }

    }

    rv = BFDIPV6_ACCESS.auth_sha1.get(unit, index, sha1);
    BCMDNX_IF_ERR_EXIT(rv);
    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure.
 *      sp    - (IN) Pointer to Simple Password info structure.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_auth_simple_password_set(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
#ifdef BCM_CMICM_SUPPORT

    shr_bfd_msg_ctrl_auth_sp_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int rv = 0;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(sp);

    AUTH_SP_INDEX_CHECK(unit, index);

    /* Password length MUST be from 1 to 16 bytes */
    if ((sp->length == 0) ||
        ( sp->length > _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH)) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "BFD simple password length must be from 1 to 16 bytes\n")));
        return BCM_E_PARAM;
    }

    /* Set control message data */
    sal_memset(&msg_auth, 0, sizeof(msg_auth));
    msg_auth.index  = index;
    msg_auth.length = sp->length;
    sal_memcpy(msg_auth.password, sp->password,
               _SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH);

    /* Pack control message data into DMA buffer */
    buffer     = bfd_ipv6_dpp_info->dma_buffer;
    buffer_ptr = bfd_sdk_dpp_msg_ctrl_auth_sp_pack(buffer, &msg_auth);
    buffer_len = buffer_ptr - buffer;

    /* Send BFD Auth SimplePassword Set message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive(unit,
                                        MOS_MSG_SUBCLASS_BFD_AUTH_SP_SET,
                                        buffer_len, 0,
                                        MOS_MSG_SUBCLASS_BFD_AUTH_SP_SET_REPLY,
                                        &reply_len));

    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    rv = BFDIPV6_ACCESS.auth_sp.set(unit, index, sp);
    BCMDNX_IF_ERR_EXIT(rv);
    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve.
 *      sp    - (IN/OUT) Pointer to Simple Password info structure to
 *              receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to SHA1 information structure
 *      BCM_E_MEMORY   Unable to allocate memory
 *      BCM_E_INTERNAL Unable to obtain/release resource lock /
 *                     Failed to read or write register
 * Notes:
 */
int
bcm_petra_bfd_uc_auth_simple_password_get(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
#ifdef BCM_CMICM_SUPPORT

    shr_bfd_msg_ctrl_auth_sp_t msg_auth;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    int rv = 0;
    int i;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);

    BCMDNX_INIT_FUNC_DEFS;

    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(sp);

    AUTH_SP_INDEX_CHECK(unit, index);

    /* If warmboot, get authentication data from uController, else use cache */
    if (SOC_WARM_BOOT(unit)) {

        /* Send BFD Auth SimplePassword Get message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_msg_send_receive
             (unit,
              MOS_MSG_SUBCLASS_BFD_AUTH_SP_GET,
              index, 0,
              MOS_MSG_SUBCLASS_BFD_AUTH_SP_GET_REPLY,
              &reply_len));

        /* Unpack control message data from DMA buffer */
        sal_memset(&msg_auth, 0, sizeof(msg_auth));
        buffer     = bfd_ipv6_dpp_info->dma_buffer;
        buffer_ptr = bfd_sdk_dpp_msg_ctrl_auth_sp_unpack(buffer, &msg_auth);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            return BCM_E_INTERNAL;
        }
        rv = BFDIPV6_ACCESS.auth_sp.length.set(unit, index, msg_auth.length);
        BCMDNX_IF_ERR_EXIT(rv);
        for(i=0;i<_SHR_BFD_AUTH_SIMPLE_PASSWORD_KEY_LENGTH;i++) {
        rv = BFDIPV6_ACCESS.auth_sp.password.set(unit, index, i, msg_auth.password[i]);
        BCMDNX_IF_ERR_EXIT(rv);
        }
    }

    rv = BFDIPV6_ACCESS.auth_sp.get(unit, index, sp);

    return BCM_E_NONE;

    exit:
        BCMDNX_FUNC_RETURN;
#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_dump_trace
 * Purpose:
 *      Dump the BFD stack trace
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      None
 * Notes:
 */
void
_bcm_petra_bfd_uc_dump_trace(int unit)
{
#ifdef BCM_CMICM_SUPPORT
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);
    uint16                 trace[BFD_TRACE_LOG_MAX_SIZE + SHR_BFD_TRACE_LOG_METADATA_SIZE];
    char                  *filename[] = SHR_BFD_FILENAMES;
    uint32                 w32;
    uint8                  file;
    uint16                 line;
    uint32                 pcie_addr, uc_addr;
    int                    idx, cur_idx;

    if ((bfd_ipv6_dpp_info == NULL) || (bfd_ipv6_dpp_info->trace_addr == 0)) {
        LOG_ERROR(BSL_LS_BCM_BFD,
                  (BSL_META_U(unit,
                              "Trace not enabled \n")));
        return;
    }

    sal_memset(trace, 0,
               sizeof(uint16) * (BFD_TRACE_LOG_MAX_SIZE +
                                 SHR_BFD_TRACE_LOG_METADATA_SIZE));

    /* We read 32 bits and each entry is 16 bit */
    for (idx = 0;
         idx < (BFD_TRACE_LOG_MAX_SIZE + SHR_BFD_TRACE_LOG_METADATA_SIZE);
         idx += (sizeof(uint32) / sizeof(uint16))) {

        uc_addr   = bfd_ipv6_dpp_info->trace_addr + (idx * sizeof(uint16));
        pcie_addr = soc_uc_addr_to_pcie(unit, bfd_ipv6_dpp_info->uc_num, uc_addr);
        w32       = soc_uc_mem_read(unit, pcie_addr);

        trace[idx]     = w32 & 0xFFFF;
        trace[idx + 1] = (w32 >> 16) & 0xFFFF;
    }

    idx = cur_idx = SHR_BFD_TRACE_LOG_CUR_IDX(trace);
    do {
        file = trace[idx] >> 12;
        line = trace[idx] & 0xFFF;

        if (file >= SHR_BFD_FILENAME_MAX) {
            LOG_ERROR(BSL_LS_BCM_BFD,
                      (BSL_META_U(unit,
                                  "Trace dump failed \n")));
            break;
        }

        if (line != 0) {
            LOG_CLI((BSL_META("%s : %4u \n"), filename[file], line));
        }

        idx = SHR_BFD_TRACE_LOG_IDX_DECR(trace, idx);
    } while(idx != cur_idx);
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_discard_stat_get
 * Purpose:
 *      Get global discarded packet statistics for BFD IPv6 FW.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_PARAM    Null pointer to discarded structure.
 *      BCM_E_INTERNAL Unable to reach FIRMWARE
 * Notes:
 */
int
bcm_petra_bfd_uc_discard_stat_get(int unit,
                             bcm_bfd_discard_stat_t *discarded_info)
{
#ifdef BCM_CMICM_SUPPORT

    bfd_sdk_msg_ctrl_discard_stat_get_t msg_discard_stat;
    uint8 *buffer, *buffer_ptr;
    uint16 buffer_len, reply_len;
    bfd_ipv6_no_wb_info_t *bfd_ipv6_dpp_info = BFD_IPV6_DPP_INFO(unit);


    BFD_UC_FEATURE_INIT_CHECK(unit);

    PARAM_NULL_CHECK(discarded_info);



        /* Send BFD Discard Get message to uC */
        BCM_IF_ERROR_RETURN
            (_bcm_petra_bfd_uc_msg_send_receive
             (unit,
              MOS_MSG_SUBCLASS_BFD_DISCARD_STAT_GET,
              0, 0,
              MOS_MSG_SUBCLASS_BFD_DISCARD_STAT_GET_REPLY,
              &reply_len));

        /* Unpack control message data from DMA buffer */
        sal_memset(&msg_discard_stat, 0, sizeof(msg_discard_stat));
        buffer = bfd_ipv6_dpp_info->dma_buffer;
        buffer_ptr = bfd_sdk_dpp_msg_ctrl_discard_stat_get_unpack(buffer, &msg_discard_stat);
        buffer_len = buffer_ptr - buffer;

        if (reply_len != buffer_len) {
            return BCM_E_INTERNAL;
        }

        discarded_info->bfd_ver_err = msg_discard_stat.bfd_ver_err;
        discarded_info->bfd_len_err = msg_discard_stat.bfd_len_err;
        discarded_info->bfd_detect_mult = msg_discard_stat.bfd_detect_mult;
        discarded_info->bfd_my_disc = msg_discard_stat.bfd_my_disc;
        discarded_info->bfd_p_f_bit = msg_discard_stat.bfd_p_f_bit;
        discarded_info->bfd_m_bit = msg_discard_stat.bfd_m_bit;
        discarded_info->bfd_auth_type_mismatch = msg_discard_stat.bfd_auth_type_mismatch;
        discarded_info->bfd_auth_simple_err = msg_discard_stat.bfd_auth_simple_err;
        discarded_info->bfd_auth_m_sha1_err = msg_discard_stat.bfd_auth_m_sha1_err;
        discarded_info->bfd_sess_mismatch = msg_discard_stat.bfd_sess_mismatch;

        return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}

/*
 * Function:
 *      bcm_petra_bfd_uc_discard_stat_set
 * Purpose:
 *      Reset global discarded packet statistics for BFD IPv6 FW.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info  - (IN) Pointer to SHA1 info structure to set the data. Ignored.
 * Returns:
 *      BCM_E_NONE     Operation completed successfully
 *      BCM_E_INTERNAL Unable to reach FIRMWARE
 * Notes:
 */
int
bcm_petra_bfd_uc_discard_stat_set(int unit,
        bcm_bfd_discard_stat_t *discarded_info)
{
#ifdef BCM_CMICM_SUPPORT

    uint16 reply_len;


    BFD_UC_FEATURE_INIT_CHECK(unit);

    /* Send BFD Discard Set message to uC */
    BCM_IF_ERROR_RETURN
        (_bcm_petra_bfd_uc_msg_send_receive
         (unit,
          MOS_MSG_SUBCLASS_BFD_DISCARD_STAT_SET,
          0, 0,
          MOS_MSG_SUBCLASS_BFD_DISCARD_STAT_SET_REPLY,
          &reply_len));
    if (reply_len != 0) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;

#else  /* BCM_CMICM_SUPPORT */
    return BCM_E_UNAVAIL;
#endif /* BCM_CMICM_SUPPORT */
}
