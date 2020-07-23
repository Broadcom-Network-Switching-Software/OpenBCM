/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IFA - In-band Flow Analyzer Monitoring Embedded Application APP interface
 * Purpose: API to configure IFA embedded app interface.
 */

#if defined(INCLUDE_IFA)

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/uc.h>
#include <soc/loopback.h>
#include <shared/alloc.h>
#include <soc/shared/ifa.h>
#include <soc/shared/ifa_msg.h>
#include <soc/shared/ifa_pack.h>
#include <soc/shared/shr_pkt.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/ifa.h>

#include <bcm_int/esw/ifa.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/ifa_feature.h>
#include <bcm_int/esw/ifa_sdk_pack.h>
#include <bcm_int/esw/ifa_sdk_msg.h>

#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
#include <soc/flexport/trident3/trident3_flexport_defines.h>
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */

/* In-band Flow Analyzer global information */
_bcm_int_ifa_info_t *ifa_global_info[BCM_MAX_NUM_UNITS] = {0};

/*Global to hold the ifa firmware version for feature compatibility check */
uint32 ifa_firmware_version = 0;

/* List of ifa collector info*/
bcm_ifa_collector_info_t ifa_collector_info[BCM_MAX_NUM_UNITS];

/* List of ifa collector info*/
bcm_ifa_config_info_t ifa_config_info[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_xgs5_ifa_convert_uc_error
 * Purpose:
 *      Converts uController error code to corresponding BCM error code.
 * Parameters:
 *      uc_rv  - (IN) uController error code to convert.
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 * Notes:
 */
STATIC int
_bcm_xgs5_ifa_convert_uc_error(uint32 uc_rv)
{
    int rv = BCM_E_NONE;

    switch(uc_rv) {
    case SHR_IFA_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_IFA_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_IFA_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_IFA_UC_E_UNIT:
        rv = BCM_E_UNIT;
        break;
    case SHR_IFA_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_IFA_UC_E_EMPTY:
        rv = BCM_E_EMPTY;
        break;
    case SHR_IFA_UC_E_FULL:
        rv = BCM_E_FULL;
        break;
    case SHR_IFA_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_IFA_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_IFA_UC_E_TIMEOUT:
        rv = BCM_E_TIMEOUT;
        break;
    case SHR_IFA_UC_E_BUSY:
        rv = BCM_E_BUSY;
        break;
    case SHR_IFA_UC_E_FAIL:
        rv = BCM_E_FAIL;
        break;
    case SHR_IFA_UC_E_DISABLED:
        rv = BCM_E_DISABLED;
        break;
    case SHR_IFA_UC_E_BADID:
        rv = BCM_E_BADID;
        break;
    case SHR_IFA_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_IFA_UC_E_CONFIG:
        rv = BCM_E_CONFIG;
        break;
    case SHR_IFA_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    case SHR_IFA_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_IFA_UC_E_PORT:
        rv = BCM_E_PORT;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    return rv;
}

/*
 * Functions:
 *      _bcm_xgs5_ifa_<header>_get
 * Purpose:
 *      The following set of _get() functions builds a given header for
 *      a given collector.
 * Parameters:
 *      collector_info -  (IN) Pointer to Collector information structure.
 *      <... other IN args ...>
 *      <header>  - (OUT) Returns header.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcm_xgs5_ifa_int_header_get(int unit,
                             bcm_ifa_config_info_t *config_info,
                             shr_int_header_t *int_h)
{
    /* Set INT */
    sal_memset(int_h, 0, sizeof(*int_h));
    int_h->probemarker1     = config_info->probemarker_1;
    int_h->probemarker2     = config_info->probemarker_2;
    int_h->maximumlength    = config_info->max_payload_length;
    int_h->hoplimit         = config_info->hop_limit;

    /* INT Header below fields fixed in case of IFA eapps */
    int_h->ver                  = 1;
    int_h->type                 = 1;
    int_h->flags                = 0;
    int_h->requestvector        = 0xFFFFFFFF;
    int_h->hopcount             = 0;
    int_h->currentlength        = 0;

    /* INT header sequence number update by R5 */
    int_h->senderhandle         = 0;
    int_h->seqnumber            = 0;

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa_lb_header_get(
                            bcm_ifa_config_info_t *config_info,
                            shr_lb_header_t *lb)
{
    /* LB Header below fields fixed in case of IFA eapps */
    sal_memset(lb, 0, sizeof(*lb));
    lb->start               = 0xFB;
    lb->common_hdr1         = 1;
    lb->source_type         = 1;
    lb->visibility          = 1;
    lb->pkt_profile         = 3;

    /* LB header other fields are update by R5 */
    return BCM_E_NONE;
}


STATIC int
_bcm_xgs5_ifa_udp_header_get(
                             bcm_ifa_collector_info_t *collector_info,
                             shr_udp_header_t *udp)
{
    /* Set UDP */
    sal_memset(udp, 0, sizeof(*udp));
    udp->src = collector_info->udp.src_port;
    udp->dst = collector_info->udp.dst_port;
    /* Do not set UDP length since this will be filled by
     * EAPP.
     */
    udp->length = 0;
    udp->sum = 0;    /* Calculated later */

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa_ipv6_header_get(uint8 protocol,
        uint32 f_label, uint8 t_class, uint8 hop_limit,
        bcm_ip6_t src_ip_addr, bcm_ip6_t dst_ip_addr,
        shr_ipv6_header_t *ip)
{
    sal_memset(ip, 0, sizeof(*ip));
    ip->version  = SHR_IPV6_VERSION;
    ip->next_header = protocol;
    ip->t_class = t_class;
    ip->hop_limit = hop_limit;
    ip->f_label = f_label;
    /* Dont fill in length since length will be calculated by
     * EAPP.
     */
    ip->p_length   = 0;

    /* Destination and Source IP address */
    sal_memcpy(ip->src, src_ip_addr, sizeof(bcm_ip6_t));
    sal_memcpy(ip->dst, dst_ip_addr, sizeof(bcm_ip6_t));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa_ipv4_header_get(uint8 protocol,
        uint8 tos, uint8 ttl,
        bcm_ip_t src_ip_addr, bcm_ip_t dst_ip_addr,
        shr_ipv4_header_t *ip)
{
    uint8 buffer[SHR_IPV4_HEADER_LENGTH];

    sal_memset(ip, 0, sizeof(*ip));
    ip->version  = SHR_IPV4_VERSION;
    ip->h_length = SHR_IPV4_HEADER_LENGTH_WORDS;
    ip->dscp     = tos;
    ip->ecn      = 0;
    /* Dont fill in length since length will be calculated by
     * EAPP.
     */
    ip->length   = 0;
    ip->id       = 0;
    ip->flags    = 0;
    ip->f_offset = 0;
    ip->ttl      = ttl;
    ip->protocol = protocol;
    ip->sum      = 0;
    ip->src      = src_ip_addr;
    ip->dst      = dst_ip_addr;

    /* Calculate IP header checksum */
    shr_ipv4_header_pack(buffer, ip);
    ip->sum = 0;

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa_ip6_headers_get(bcm_ifa_collector_info_t *collector_info,
                              uint16 *etype,
                              shr_ipv6_header_t *ipv6)
{
    int ip_protocol;

    ip_protocol = SHR_IP_PROTOCOL_UDP;
    *etype = SHR_L2_ETYPE_IPV6;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa_ipv6_header_get(ip_protocol,
                                       collector_info->ipv6.flow_label,
                                       collector_info->ipv6.traffic_class,
                                       collector_info->ipv6.hop_limit,
                                       collector_info->ipv6.src_ip,
                                       collector_info->ipv6.dst_ip,
                                       ipv6));

    return BCM_E_NONE;
}


STATIC int
_bcm_xgs5_ifa_ip4_headers_get(bcm_ifa_collector_info_t *collector_info,
        uint16 *etype,
        shr_ipv4_header_t *ipv4)
{
    int ip_protocol;

    ip_protocol = SHR_IP_PROTOCOL_UDP;
    *etype = SHR_L2_ETYPE_IPV4;

    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa_ipv4_header_get(ip_protocol,
                                       collector_info->ipv4.dscp,
                                       collector_info->ipv4.ttl,
                                       collector_info->ipv4.src_ip,
                                       collector_info->ipv4.dst_ip,
                                       ipv4));

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa_l2_header_get(bcm_ifa_collector_info_t *collector_info,
        uint16 etype, shr_l2_header_t *l2)
{
    sal_memset(l2, 0, sizeof(*l2));

    /* Destination and Source MAC */
    sal_memcpy(l2->dst_mac, collector_info->eth.dst_mac, SHR_MAC_ADDR_LENGTH);
    sal_memcpy(l2->src_mac, collector_info->eth.src_mac, SHR_MAC_ADDR_LENGTH);

    /*
     * VLAN Tag(s)
     */
    if (collector_info->eth.vlan_tag_structure == BCM_IFA_COLLECTOR_ETH_HDR_UNTAGGED) {
        /* Set to 0 to indicate untagged */
        l2->outer_vlan_tag.tpid = 0;
        l2->inner_vlan_tag.tpid = 0;
    } else {
        /* Fill outer vlan */
        if ((collector_info->eth.vlan_tag_structure == BCM_IFA_COLLECTOR_ETH_HDR_SINGLE_TAGGED) ||
            (collector_info->eth.vlan_tag_structure == BCM_IFA_COLLECTOR_ETH_HDR_DOUBLE_TAGGED))
        {
            l2->outer_vlan_tag.tpid     = collector_info->eth.outer_tpid;
            l2->outer_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector_info->eth.outer_vlan_tag);
            l2->outer_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector_info->eth.outer_vlan_tag);
        } else {
            return BCM_E_PARAM;
        }
        /* Fill inner vlan if double tagged */
        if (collector_info->eth.vlan_tag_structure == BCM_IFA_COLLECTOR_ETH_HDR_DOUBLE_TAGGED) {
            l2->inner_vlan_tag.tpid     = collector_info->eth.inner_tpid;
            l2->inner_vlan_tag.tci.prio = BCM_VLAN_CTRL_PRIO(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.cfi  = BCM_VLAN_CTRL_CFI(collector_info->eth.inner_vlan_tag);
            l2->inner_vlan_tag.tci.vid  = BCM_VLAN_CTRL_ID(collector_info->eth.inner_vlan_tag);
        } else {
            l2->inner_vlan_tag.tpid = 0;  /* Set to 0 to indicate not inner tagged */
        }
    }

    /* Ether Type */
    l2->etype = etype;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa_eapp_collector_encap_build_pack
 * Purpose:
 *      Builds and packs the IFA export packet
 *      encapsulation for a given collector
 * Parameters:
 *      collector_info  - (IN) Pointer to collector info structure.
 *      collector_set_msg - (OUT) Message which contains the encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning encapsulation includes only all the
 *      encapsulation headers and does not include control packet.
 */
STATIC int
_bcm_xgs5_ifa_eapp_collector_encap_build_pack(
        bcm_ifa_collector_info_t *collector_info,
        shr_ifa_msg_ctrl_collector_info_t *collector_set_msg)
{
    uint8               *buffer  = collector_set_msg->encap;
    uint8               *cur_ptr;
    uint16              etype = 0;
    shr_udp_header_t    udp;
    shr_ipv4_header_t   ipv4;
    shr_ipv6_header_t   ipv6;
    shr_l2_header_t     l2;
    uint16              ip_offset = 0;
    uint16              udp_offset = 0;

    sal_memset(&udp, 0, sizeof(udp));
    sal_memset(&ipv4, 0, sizeof(ipv4));
    sal_memset(&ipv6, 0, sizeof(ipv6));
    sal_memset(&l2, 0, sizeof(l2));

    /*
     * Get necessary headers/labels information.
     *
     * Following order is important since some headers/labels
     * may depend on previous header/label information.
     */
    if ((collector_info->transport_type == bcmIfaCollectorTransportTypeIpv4Udp) ||
        (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv6Udp)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ifa_udp_header_get(collector_info, &udp));
    }

    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv4Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ifa_ip4_headers_get(collector_info,
                                           &etype,
                                           &ipv4));
    }

    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv6Udp) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs5_ifa_ip6_headers_get(collector_info,
                                          &etype,
                                          &ipv6));
    }

    /* Always build L2 Header */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa_l2_header_get(collector_info, etype, &l2));

    /*
     * Pack header/labels into given buffer (network packet format).
     *
     * Following packing order must be followed to correctly
     * build the packet encapsulation.
     */
    cur_ptr = buffer;

    /* L2 Header is always present */
    cur_ptr = shr_l2_header_pack(cur_ptr, &l2);

    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv6Udp) {
        /*
         * IP offset
         */
        ip_offset = cur_ptr - buffer;
        cur_ptr = shr_ipv6_header_pack(cur_ptr, &ipv6);
    }

    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv4Udp) {
        /*
         * IP offset
         */
        ip_offset = cur_ptr - buffer;
        cur_ptr = shr_ipv4_header_pack(cur_ptr, &ipv4);
    }

    if ((collector_info->transport_type == bcmIfaCollectorTransportTypeIpv4Udp) ||
        (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv6Udp)) {
        /*
         * UDP offset
         */
        udp_offset = cur_ptr - buffer;
        cur_ptr = shr_udp_header_pack(cur_ptr, &udp);
    }


    /* Set IFA export pkt encapsulation length */
    collector_set_msg->encap_length = cur_ptr - buffer;

    /* Set IFA export pkt IP base offset */
    collector_set_msg->ip_offset = ip_offset;

    /* Set IFA export pkt IP Base checksum */
    collector_set_msg->ip_base_checksum =
        shr_initial_chksum_get(SHR_IPV4_HEADER_LENGTH, buffer + ip_offset);

    /* Set IFA export pkt UDP Base checksum */
    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv4Udp) {
        collector_set_msg->udp_base_checksum =
            shr_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);
    }

    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv6Udp) {
        collector_set_msg->udp_base_checksum =
            shr_udp_initial_checksum_get(1, NULL, &ipv6, NULL, &udp);
    }

    /* Set IFA export pkt UDP base offset */
    collector_set_msg->udp_offset = udp_offset;
    collector_set_msg->mtu        = collector_info->mtu;

    collector_set_msg->transport_type        = collector_info->transport_type;
    collector_set_msg->outer_vlan_tag        = collector_info->eth.outer_vlan_tag;
    collector_set_msg->inner_vlan_tag        = collector_info->eth.inner_vlan_tag;
    collector_set_msg->outer_tpid            = collector_info->eth.outer_tpid;
    collector_set_msg->inner_tpid            = collector_info->eth.inner_tpid;
    collector_set_msg->vlan_tag_structure    = collector_info->eth.vlan_tag_structure;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa_eapp_loopback_port_set
 * Purpose:
 *      Builds and packs Device Id INT header
 * Parameters:
 *      config_info     - (IN) Device Id
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning encapsulation includes only INT headers
 */
STATIC int
_bcm_xgs5_ifa_eapp_loopback_port_set(int unit, bcm_port_t lb_port)
{
    int rv = BCM_E_NONE;
#if defined(INCLUDE_L3)
    int     mod_id, pp_port, tgid, id;
    bcm_gport_t gport;
    int pipe_num;
    int loopback_port;
    source_trunk_map_modbase_entry_t modbase_entry;
    int i, lb_idx;
    uint32 src_ent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 lb_src_ent[SOC_MAX_MEM_FIELD_WORDS];
    int lport_index = 0;
    int lb_lport_index = 0;

    BCM_IF_ERROR_RETURN(bcm_esw_port_gport_get(unit, lb_port, &gport));
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, gport, &mod_id, &pp_port,
                                               &tgid, &id));

    /*
     * Program the SOURCE_TRUNK_MAP with packet
     * processing port
     */
    SOC_IF_ERROR_RETURN(
        soc_port_pipe_get(unit, pp_port, &pipe_num));
    loopback_port = soc_loopback_lbport_num_get(unit, pipe_num);
    if (loopback_port == -1) {
        return BCM_E_PARAM;
    }
    sal_memset(src_ent, 0, sizeof(src_ent));
    sal_memset(lb_src_ent, 0, sizeof(lb_src_ent));
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, ING_DEVICE_PORTm, MEM_BLOCK_ANY, pp_port,
         src_ent));
    lport_index =
        soc_mem_field32_get(unit, ING_DEVICE_PORTm, src_ent, LPORT_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, ING_DEVICE_PORTm, MEM_BLOCK_ANY, loopback_port,
         lb_src_ent));
    lb_lport_index=
        soc_mem_field32_get(unit, ING_DEVICE_PORTm, lb_src_ent, LPORT_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, SOURCE_TRUNK_MAP_MODBASEm, MEM_BLOCK_ANY,
                mod_id, &modbase_entry));

    i = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_MODBASEm,
                    &modbase_entry, BASEf) + pp_port;
    lb_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_MODBASEm,
                    &modbase_entry, BASEf) + loopback_port;

    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                i, PP_PORT_NUMf, pp_port));
    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                i, LPORT_PROFILE_IDXf, lport_index));
    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                lb_idx, PP_PORT_NUMf, loopback_port));
    BCM_IF_ERROR_RETURN
        (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                                lb_idx, LPORT_PROFILE_IDXf, lb_lport_index));
    return rv;
#else
    return rv;
#endif
}

/*
 * Function:
 *      _bcm_xgs5_ifa_eapp_int_encap_build_pack
 * Purpose:
 *      Builds and packs the IFA INT header
 * Parameters:
 *      config_info     - (IN) Pointer to config info structure.
 *      config_info_msg - (OUT) Message which contains the int header encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning encapsulation includes only INT headers
 */
STATIC int
_bcm_xgs5_ifa_eapp_int_encap_build_pack(int unit,
        bcm_ifa_config_info_t *config_info,
        ifa_sdk_msg_ctrl_config_info_t *config_info_msg)
{
    uint8               *buffer  = config_info_msg->int_encap;
    uint8               *cur_ptr;
    shr_int_header_t    int_h;

    sal_memset(&int_h, 0, sizeof(int_h));
    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa_int_header_get(unit, config_info, &int_h));

    /*
     * Pack header into given buffer (network packet format).
     */
    cur_ptr = buffer;

    /* INT Header is always present */
    cur_ptr = shr_int_header_pack(cur_ptr, &int_h);

    /* Set IFA export pkt encapsulation length */
    config_info_msg->int_encap_length = cur_ptr - buffer;

    if (config_info_msg->int_encap_length != SHR_INT_HEADER_LENGTH) {
        LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                        "IFA(unit %d) Info: update INT header"), unit));
        return (BCM_E_INIT);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs5_ifa_eapp_lb_encap_build_pack
 * Purpose:
 *      Builds and packs the IFA LB Header
 * Parameters:
 *      config_info     - (IN) Pointer to config info structure.
 *      config_info_msg - (OUT) Message which contains the int header encapsulation.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      The returning encapsulation includes only INT headers
 */
STATIC int
_bcm_xgs5_ifa_eapp_lb_encap_build_pack(int unit,
        bcm_ifa_config_info_t *config_info,
        ifa_sdk_msg_ctrl_config_info_t *config_info_msg)
{
    uint8               *buffer  = config_info_msg->lb_encap;
    uint8               *cur_ptr;
    shr_lb_header_t     lb;

    sal_memset(&lb, 0, sizeof(lb));
    BCM_IF_ERROR_RETURN
        (_bcm_xgs5_ifa_lb_header_get(config_info, &lb));

    /*
     * Pack header into given buffer (network packet format).
     */
    cur_ptr = buffer;

    /* LoopBack Header is always present */
    cur_ptr = shr_lb_header_pack(cur_ptr, &lb);

    /* Set IFA export pkt encapsulation length */
    config_info_msg->lb_encap_length = cur_ptr - buffer;

    if (config_info_msg->lb_encap_length != SHR_LB_HEADER_LENGTH) {
        LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                        "IFA(unit %d) Info: update LB header"), unit));
        return (BCM_E_INIT);
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_xgs5_ifa_msg_send_receive(int unit, uint8 s_subclass,
                               uint16 s_len, uint32 s_data,
                               uint8 r_subclass, uint16 *r_len,
                               sal_usecs_t timeout)
{
    int rv;
    mos_msg_data_t send, reply;
    uint32 uc_rv;
    uint8 *dma_buffer;
    int dma_buffer_len;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    sal_paddr_t dma_paddr = (sal_paddr_t)0;

    dma_buffer = ifa_info->dma_buffer;
    dma_buffer_len = ifa_info->dma_buffer_len;

    /*
     * Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive.
     */
    if (MOS_MSG_DMA_MSG(s_subclass) ||
            MOS_MSG_DMA_MSG(r_subclass)) {
        dma_paddr = soc_cm_l2p(unit, dma_buffer);
    }
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
    send.s.mclass = MOS_MSG_CLASS_IFA;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    if (MOS_MSG_DMA_MSG(s_subclass) ||
            MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(PTR_TO_INT(dma_paddr));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, ifa_info->uc_num,
                                      &send, &reply,
                                      timeout);
    if (rv != SOC_E_NONE){
        return BCM_E_INTERNAL;
    }

    /* Convert IFA uController error code to BCM */
    uc_rv  = bcm_ntohl(reply.s.data);
    rv     = _bcm_xgs5_ifa_convert_uc_error(uc_rv);

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != MOS_MSG_CLASS_IFA) ||
       (reply.s.subclass != r_subclass)))
    {
        return BCM_E_INTERNAL;
    }
    return rv;
}

int _bcm_xgs5_ifa_eapp_send_ctrl_init_msg (int unit)
{
    shr_ifa_msg_ctrl_init_t ctrl_init_msg;
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);

    sal_memset(&ctrl_init_msg, 0, sizeof(ctrl_init_msg));
    ctrl_init_msg.rx_channel = BCM_IFA_EAPP_RX_CHANNEL;
    ctrl_init_msg.tx_channel = BCM_IFA_EAPP_TX_CHANNEL;

    buffer_req      = ifa_info->dma_buffer;
    buffer_ptr      = shr_ifa_msg_ctrl_init_pack(buffer_req, &ctrl_init_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa_msg_send_receive(unit, MOS_MSG_SUBCLASS_IFA_INIT,
                buffer_len, 0,
                MOS_MSG_SUBCLASS_IFA_INIT_REPLY, &reply_len,
                SHR_IFA_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/******************************************************
 *                                                     *
 *           IFA EMBEDDED APP APIs                     *
 */

int bcm_xgs5_ifa_eapp_init(int unit)
{
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    int rv = BCM_E_NONE;
    int uc = 0;
    int status;
    bcm_ifa_collector_info_t *collector_info_node = (&(ifa_collector_info[unit]));
    bcm_ifa_config_info_t *config_info_node = (&(ifa_config_info[unit]));

    /* Init the app */
    /*
     * Start IFA application in BTE (Broadcom Task Engine) uController,
     * if not already running (warm boot).
     * Determine which uController is running IFA  by choosing the first
     * uC that returns successfully.
     */
    for (uc = 0; uc < CMICM_NUM_UCS; uc++) {
        rv = soc_uc_status(unit, uc, &status);
        if ((rv == SOC_E_NONE) && (status != 0)) {
            rv = soc_cmic_uc_appl_init(unit, uc, MOS_MSG_CLASS_IFA,
                                       SHR_IFA_MAX_UC_MSG_TIMEOUT,
                                       IFA_SDK_VERSION,
                                       IFA_UC_MIN_VERSION,
                                       NULL, NULL);
            if (SOC_E_NONE == rv) {
                /* IFA started successfully */
                break;
            }
        }
    }

    if (uc >= CMICM_NUM_UCS) {
        /* Could not find or start IFA appl */
        return BCM_E_NONE;
    }
    ifa_info->uc_num             = uc;

    /* Detach if already initialized */
    if (!soc_feature(unit, soc_feature_ifa)) {
        BCM_IF_ERROR_RETURN(bcm_xgs5_ifa_eapp_detach(unit));
        return BCM_E_INIT;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Send the init config to UKERNEL */
        rv = _bcm_xgs5_ifa_eapp_send_ctrl_init_msg(unit);
        if (BCM_FAILURE(rv)) {
            BCM_IF_ERROR_RETURN(bcm_xgs5_ifa_eapp_detach(unit));
            return rv;
        }
        sal_memset(collector_info_node, 0, sizeof(bcm_ifa_collector_info_t));
        sal_memset(config_info_node, 0, sizeof(bcm_ifa_config_info_t));
    } else {
        /* Get the Collector info */
        rv = bcm_xgs5_ifa_eapp_collector_info_get_msg(unit, collector_info_node);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Get the Config info */
        rv = bcm_xgs5_ifa_eapp_config_info_get_msg(unit, config_info_node);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    ifa_info->status = 1;
    return BCM_E_NONE;
}

int bcm_xgs5_ifa_eapp_detach(int unit)
{
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    int                          rv = BCM_E_NONE;
    int                          status = 0;
    uint16                       reply_len = 0;

    if (!SOC_WARM_BOOT(unit)) {
        /* Un Init the app */
        SOC_IF_ERROR_RETURN(soc_uc_status(unit, ifa_info->uc_num, &status));
        if (status) {
            rv = _bcm_xgs5_ifa_msg_send_receive(unit,
                    MOS_MSG_SUBCLASS_IFA_SHUTDOWN,
                    0, 0,
                    MOS_MSG_SUBCLASS_IFA_SHUTDOWN_REPLY,
                    &reply_len,
                    SHR_IFA_MAX_UC_MSG_TIMEOUT);
            if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                return BCM_E_INTERNAL;
            }
        }
    }

    /*
     * Free DMA buffer
 */
    if (ifa_info->dma_buffer != NULL) {
        soc_cm_sfree(unit, ifa_info->dma_buffer);
    }
    ifa_info->status = 0;

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_xgs5_ifa_collector_set
 * Purpose:
 *     Set a collector information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_info - (IN) Collector informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_xgs5_ifa_collector_set(int unit,
                           bcm_ifa_collector_info_t *collector_info)
{
    bcm_ifa_collector_info_t *collector_info_node = (&(ifa_collector_info[unit]));

    if (collector_info_node->mtu != 0) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA(unit %d) Info: already exists \n"), unit));
        return BCM_E_EXISTS;
    }

    sal_memcpy(collector_info_node, collector_info,
               sizeof(bcm_ifa_collector_info_t));

    LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                    "IFA(unit %d) Info: %s \n"), unit, FUNCTION_NAME()));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_xgs5_ifa_collector_modify
 * Purpose:
 *     Set a collector information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_info - (IN) Collector informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_xgs5_ifa_collector_modify(int unit,
                              bcm_ifa_collector_info_t *collector_info)
{
    bcm_ifa_collector_info_t *collector_info_node = (&(ifa_collector_info[unit]));

    if (collector_info_node->mtu == 0) {
        LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA(unit %d) Info: collector does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(collector_info_node, collector_info,
               sizeof(bcm_ifa_collector_info_t));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_xgs5_ifa_collector_get
 * Purpose:
 *     Set a collector information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     collector_info - (OUT) Collector informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_xgs5_ifa_collector_get(int unit,
                           bcm_ifa_collector_info_t *collector_info)
{
    bcm_ifa_collector_info_t *collector_info_node = (&(ifa_collector_info[unit]));

    if (collector_info_node->mtu == 0) {
        LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA(unit %d) Info: collector does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(collector_info, (&(ifa_collector_info[unit])),
               sizeof(bcm_ifa_collector_info_t));
    return (BCM_E_NONE);
}

int bcm_xgs5_ifa_eapp_collector_set_msg(int unit,
                                        bcm_ifa_collector_info_t *collector_info)
{
    shr_ifa_msg_ctrl_collector_info_t collector_set_msg;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;

    sal_memset(&collector_set_msg, 0, sizeof(collector_set_msg));
    if (collector_info->mtu) {
        rv = _bcm_xgs5_ifa_eapp_collector_encap_build_pack(
                                                           collector_info,
                                                           &collector_set_msg);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Check if there is space to insert at least the headers  */
        if ((collector_set_msg.encap_length + BCM_IFA_L2_CRC_LENGTH) >
            collector_set_msg.mtu) {
            LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                            "IFA(unit %d) Info: collector MTU \n"), unit));
            return BCM_E_PARAM;
        }
    } else {
        bcm_ifa_collector_info_t *collector_info_node = (&(ifa_collector_info[unit]));
        sal_memset(collector_info_node, 0, sizeof(bcm_ifa_collector_info_t));
    }

    buffer_req      = ifa_info->dma_buffer;
    buffer_ptr      = shr_ifa_msg_ctrl_collector_info_pack(buffer_req, &collector_set_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa_msg_send_receive(unit, MOS_MSG_SUBCLASS_IFA_COLLECTOR_SET,
                                                       buffer_len, 0,
                                                       MOS_MSG_SUBCLASS_IFA_COLLECTOR_SET_REPLY,
                                                       &reply_len,
                                                       SHR_IFA_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

/*
 * Function:
 *     bcm_xgs5_ifa_config_set
 * Purpose:
 *     Set a configuration information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     config_info   - (IN) Configuration informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_xgs5_ifa_config_set(int unit,
                        bcm_ifa_config_info_t *config_info)
{
    bcm_ifa_config_info_t *config_info_node = (&(ifa_config_info[unit]));

    if (config_info_node->max_payload_length != 0) {
        LOG_ERROR(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA(unit %d) Info: already exists \n"), unit));
        return BCM_E_EXISTS;
    }

    sal_memcpy(config_info_node, config_info,
               sizeof(bcm_ifa_config_info_t));

    LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                    "IFA(unit %d) Info: %s \n"), unit, FUNCTION_NAME()));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_xgs5_ifa_config_modify
 * Purpose:
 *     Modify a configuration information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     config_info   - (IN) Configuration informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_xgs5_ifa_config_modify(int unit,
                           bcm_ifa_config_info_t *config_info)
{
    bcm_ifa_config_info_t *config_info_node = (&(ifa_config_info[unit]));

    if (config_info_node->max_payload_length == 0) {
        LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA(unit %d) Info: configuration does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(config_info_node, config_info,
               sizeof(bcm_ifa_config_info_t));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_xgs5_ifa_config_get
 * Purpose:
 *     Get a configuration information.
 * Parameters:
 *     unit          - (IN) BCM device number
 *     config_info   - (OUT) Configuration informatin.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_xgs5_ifa_config_get(int unit,
                        bcm_ifa_config_info_t *config_info)
{
    bcm_ifa_config_info_t *config_info_node = (&(ifa_config_info[unit]));

    if (config_info_node->max_payload_length == 0) {
        LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                  "IFA(unit %d) Info: configuration does not exist"), unit));
        return (BCM_E_NOT_FOUND);
    }

    sal_memcpy(config_info, (&(ifa_config_info[unit])),
               sizeof(bcm_ifa_config_info_t));
    return (BCM_E_NONE);
}

int bcm_xgs5_ifa_eapp_config_info_msg(int unit,
                                      bcm_ifa_config_info_t *config_info)
{
    ifa_sdk_msg_ctrl_config_info_t config_info_msg;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;
    int rv = BCM_E_NONE;

    sal_memset(&config_info_msg, 0, sizeof(config_info_msg));
    if (config_info->max_payload_length) {
        bcm_port_t start_port = 1;
        bcm_port_t last_port = 1;

        config_info_msg.probemarker_1       = config_info->probemarker_1;
        config_info_msg.probemarker_2       = config_info->probemarker_2;
        config_info_msg.device_id           = config_info->device_id;

#if defined(BCM_TRIDENT3_SUPPORT)
        if (SOC_IS_TRIDENT3(unit)) {
            /* CANCUN INT header fields validation */
            if (config_info->lb_port_1 < 66) {
                config_info_msg.lb_port_1       = config_info->lb_port_1;
                config_info_msg.lb_port_2       = config_info->lb_port_2;
            } else {
                config_info_msg.lb_port_1       = config_info->lb_port_2;
                config_info_msg.lb_port_2       = config_info->lb_port_1;
            }
        }
#endif  /* BCM_TRIDENT3_SUPPORT */

#if defined(BCM_MAVERICK2_SUPPORT)
        if (SOC_IS_MAVERICK2(unit)) {
            if (config_info->lb_port_1 < 63) {
                config_info_msg.lb_port_1       = config_info->lb_port_1;
            } else {
                config_info_msg.lb_port_1       = config_info->lb_port_2;
            }
        }
#endif  /* BCM_MAVERICK2_SUPPORT */

        config_info_msg.module_id           = config_info->module_id;
        config_info_msg.hop_limit           = config_info->hop_limit;
        config_info_msg.optional_headers    = config_info->optional_headers;
        config_info_msg.max_payload_length  = config_info->max_payload_length;
        config_info_msg.rx_packet_payload_length   = config_info->rx_packet_payload_length;
        config_info_msg.true_hop_count      = config_info->true_hop_count;

        /* SYNC Ingress Device port to Source Trunk Map Table */
#if defined(BCM_TRIDENT3_SUPPORT) || defined(BCM_MAVERICK2_SUPPORT)
        if ((SOC_IS_TRIDENT3(unit)) || (SOC_IS_MAVERICK2(unit))) {
            last_port = TRIDENT3_PHY_PORTS_PER_DEV;
        }
#endif  /* BCM_TRIDENT3_SUPPORT and BCM_MAVERICK2_SUPPORT */

        for (;start_port < last_port; ++start_port) {
            rv = _bcm_xgs5_ifa_eapp_loopback_port_set(unit, start_port);
            if (BCM_FAILURE(rv)) {
                LOG_DEBUG(BSL_LS_BCM_IFA, (BSL_META_U(unit,
                          "IFA(unit %d) Info: start_port: %d not validate port "),
                          unit, start_port));
            }
        }

        rv = _bcm_xgs5_ifa_eapp_int_encap_build_pack(unit,
                        config_info, &config_info_msg);
        if (BCM_FAILURE(rv)) {
            return rv;
        }

        rv = _bcm_xgs5_ifa_eapp_lb_encap_build_pack(unit,
                config_info, &config_info_msg);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    } else {
        bcm_ifa_config_info_t *config_info_node = (&(ifa_config_info[unit]));
        sal_memset(config_info_node, 0, sizeof(bcm_ifa_config_info_t));
    }

    buffer_req      = ifa_info->dma_buffer;
    buffer_ptr      = ifa_sdk_msg_ctrl_config_info_pack(buffer_req,
                                                        &config_info_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa_msg_send_receive(unit, MOS_MSG_SUBCLASS_IFA_CONFIG_SET,
                                                       buffer_len, 0,
                                                       MOS_MSG_SUBCLASS_IFA_CONFIG_SET_REPLY,
                                                       &reply_len,
                                                       SHR_IFA_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}

int bcm_xgs5_ifa_eapp_collector_info_get_msg(int unit,
                                             bcm_ifa_collector_info_t *collector_info)
{
    shr_ifa_msg_ctrl_collector_info_t collector_msg;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0, i;

    if (collector_info == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(collector_info, 0, sizeof(bcm_ifa_collector_info_t));
    sal_memset(&collector_msg, 0, sizeof(collector_msg));

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa_msg_send_receive(unit, MOS_MSG_SUBCLASS_IFA_COLLECTOR_GET,
                                                       buffer_len, 0,
                                                       MOS_MSG_SUBCLASS_IFA_COLLECTOR_GET_REPLY, &reply_len,
                                                       SHR_IFA_MAX_UC_MSG_TIMEOUT));
    buffer_req      = ifa_info->dma_buffer;
    buffer_ptr      = shr_ifa_msg_ctrl_collector_info_unpack(buffer_req, &collector_msg);
    buffer_len      = buffer_ptr - buffer_req;

    collector_info->mtu        = collector_msg.mtu;
    /* L2 ETH and VLAN TAG Headers */
    /* Destination and Source MAC */
    for (i = 0; i < SHR_MAC_ADDR_LENGTH; i++) {
        collector_info->eth.dst_mac[i] = (uint8)collector_msg.encap[i];
    }
    for (i = 0; i < SHR_MAC_ADDR_LENGTH; i++) {
        collector_info->eth.src_mac[i] = (uint8)collector_msg.encap[(i + 6)];
    }

    collector_info->transport_type          =   collector_msg.transport_type;
    collector_info->eth.outer_vlan_tag      =   collector_msg.outer_vlan_tag;
    collector_info->eth.inner_vlan_tag      =   collector_msg.inner_vlan_tag;
    collector_info->eth.outer_tpid          =   collector_msg.outer_tpid;
    collector_info->eth.inner_tpid          =   collector_msg.inner_tpid;
    collector_info->eth.vlan_tag_structure  =   collector_msg.vlan_tag_structure;

    /* L3 IPV6 Header */
    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv6Udp) {
        collector_info->ipv6.traffic_class = (uint8)(((collector_msg.encap[collector_msg.ip_offset] & 0x0F) << 4) |
                                                     ((collector_msg.encap[collector_msg.ip_offset + 1] & 0xF0) >> 4));
        collector_info->ipv6.hop_limit  = (uint8)(collector_msg.encap[collector_msg.ip_offset + 7] & 0xFF);
        collector_info->ipv6.flow_label = (uint32)(((collector_msg.encap[collector_msg.ip_offset + 1] & 0x0F) << 16) |
                                                   ((collector_msg.encap[collector_msg.ip_offset + 2] & 0xFF) << 8)  |
                                                   (collector_msg.encap[collector_msg.ip_offset + 3] & 0xFF));
        /* Source and Destination IP */
        for (i = 0; i < 16; i++) {
            collector_info->ipv6.src_ip[i] = (uint8)(collector_msg.encap[(collector_msg.ip_offset + 8 + i)] & 0xFF);
        }
        for (i = 0; i < 16; i++) {
            collector_info->ipv6.dst_ip[i] = (uint8)(collector_msg.encap[(collector_msg.ip_offset + 24 + i)] & 0xFF);
        }
    }

    /* L3 IPV4 Header */
    if (collector_info->transport_type == bcmIfaCollectorTransportTypeIpv4Udp) {
        collector_info->ipv4.dscp = (uint8)(collector_msg.encap[collector_msg.ip_offset + 1] & 0xFF);
        collector_info->ipv4.ttl  = (uint8)(collector_msg.encap[collector_msg.ip_offset + 8] & 0xFF);
        /* Source and Destination IP */
        collector_info->ipv4.src_ip = (uint32)(((collector_msg.encap[(collector_msg.ip_offset + 12)] & 0xFF) << 24) |
                                               ((collector_msg.encap[(collector_msg.ip_offset + 13)] & 0xFF) << 16) |
                                               ((collector_msg.encap[(collector_msg.ip_offset + 14)] & 0xFF) << 8) |
                                               ((collector_msg.encap[(collector_msg.ip_offset + 15)] & 0xFF)));
        collector_info->ipv4.dst_ip = (uint32)(((collector_msg.encap[(collector_msg.ip_offset + 16)] & 0xFF) << 24) |
                                               ((collector_msg.encap[(collector_msg.ip_offset + 17)] & 0xFF) << 16) |
                                               ((collector_msg.encap[(collector_msg.ip_offset + 18)] & 0xFF) << 8) |
                                               ((collector_msg.encap[(collector_msg.ip_offset + 19)] & 0xFF)));
    }

    /* L4 UDP Header */
    collector_info->udp.src_port    =   (bcm_l4_port_t)(((collector_msg.encap[collector_msg.udp_offset] & 0xFF) << 8) |
                                                        (collector_msg.encap[(collector_msg.udp_offset + 1)] & 0xFF));
    collector_info->udp.dst_port    =   (bcm_l4_port_t)(((collector_msg.encap[(collector_msg.udp_offset + 2)] & 0xFF) << 8) |
                                                        (collector_msg.encap[(collector_msg.udp_offset + 3)] & 0xFF));
    return BCM_E_NONE;
}

int bcm_xgs5_ifa_eapp_config_info_get_msg(int unit,
                                          bcm_ifa_config_info_t *config_info)
{
    ifa_sdk_msg_ctrl_config_info_t config_info_msg;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    if (config_info == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(config_info, 0, sizeof(bcm_ifa_config_info_t));
    sal_memset(&config_info_msg, 0, sizeof(config_info_msg));

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa_msg_send_receive(unit, MOS_MSG_SUBCLASS_IFA_CONFIG_GET,
                                                       buffer_len, 0,
                                                       MOS_MSG_SUBCLASS_IFA_CONFIG_GET_REPLY, &reply_len,
                                                       SHR_IFA_MAX_UC_MSG_TIMEOUT));
    buffer_req      = ifa_info->dma_buffer;
    buffer_ptr      = ifa_sdk_msg_ctrl_config_info_unpack(buffer_req,
                                                          &config_info_msg);
    buffer_len      = buffer_ptr - buffer_req;

    config_info->probemarker_1       = config_info_msg.probemarker_1;
    config_info->probemarker_2       = config_info_msg.probemarker_2;
    config_info->device_id           = config_info_msg.device_id;
    config_info->module_id           = config_info_msg.module_id;
    config_info->hop_limit           = config_info_msg.hop_limit;
    config_info->optional_headers    = config_info_msg.optional_headers;
    config_info->lb_port_1           = config_info_msg.lb_port_1;
    config_info->lb_port_2           = config_info_msg.lb_port_2;
    config_info->max_payload_length  = config_info_msg.max_payload_length;
    config_info->rx_packet_payload_length   = config_info_msg.rx_packet_payload_length;
    config_info->true_hop_count      = config_info_msg.true_hop_count;

    return BCM_E_NONE;
}

int bcm_xgs5_ifa_eapp_stat_info_get_msg(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    shr_ifa_msg_ctrl_stat_info_t stat_data_msg;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(stat_data, 0, sizeof(bcm_ifa_stat_info_t));
    sal_memset(&stat_data_msg, 0, sizeof(stat_data_msg));

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa_msg_send_receive(unit, MOS_MSG_SUBCLASS_IFA_STAT_GET,
                                                       buffer_len, 0,
                                                       MOS_MSG_SUBCLASS_IFA_STAT_GET_REPLY, &reply_len,
                                                       SHR_IFA_MAX_UC_MSG_TIMEOUT));
    buffer_req      = ifa_info->dma_buffer;
    buffer_ptr      = shr_ifa_msg_ctrl_stat_info_unpack(buffer_req, &stat_data_msg);
    buffer_len      = buffer_ptr - buffer_req;

    stat_data->rx_pkt_cnt                       = stat_data_msg.rx_pkt_cnt;
    stat_data->tx_pkt_cnt                       = stat_data_msg.tx_pkt_cnt;
    stat_data->ifa_no_config_drop               = stat_data_msg.ifa_no_config_drop;
    stat_data->ifa_collector_not_present_drop   = stat_data_msg.ifa_collector_not_present_drop;
    stat_data->ifa_hop_cnt_invalid_drop         = stat_data_msg.ifa_hop_cnt_invalid_drop;
    stat_data->ifa_int_hdr_len_invalid_drop     = stat_data_msg.ifa_int_hdr_len_invalid_drop;
    stat_data->ifa_pkt_size_invalid_drop        = stat_data_msg.ifa_pkt_size_invalid_drop;

    return BCM_E_NONE;
}

int bcm_xgs5_ifa_eapp_stat_info_set_msg(int unit,
        bcm_ifa_stat_info_t *stat_data)
{
    shr_ifa_msg_ctrl_stat_info_t stat_data_msg;
    _bcm_int_ifa_info_t *ifa_info = IFA_INFO_GET(unit);
    uint8 *buffer_req = NULL, *buffer_ptr = NULL;
    uint16 buffer_len = 0, reply_len = 0;

    sal_memset(&stat_data_msg, 0, sizeof(stat_data_msg));
    stat_data_msg.rx_pkt_cnt                       = stat_data->rx_pkt_cnt;
    stat_data_msg.tx_pkt_cnt                       = stat_data->tx_pkt_cnt;
    stat_data_msg.ifa_no_config_drop               = stat_data->ifa_no_config_drop;
    stat_data_msg.ifa_collector_not_present_drop   = stat_data->ifa_collector_not_present_drop;
    stat_data_msg.ifa_hop_cnt_invalid_drop         = stat_data->ifa_hop_cnt_invalid_drop;
    stat_data_msg.ifa_int_hdr_len_invalid_drop     = stat_data->ifa_int_hdr_len_invalid_drop;
    stat_data_msg.ifa_pkt_size_invalid_drop        = stat_data->ifa_pkt_size_invalid_drop;

    buffer_req      = ifa_info->dma_buffer;
    buffer_ptr      = shr_ifa_msg_ctrl_stat_info_pack(buffer_req, &stat_data_msg);
    buffer_len      = buffer_ptr - buffer_req;

    BCM_IF_ERROR_RETURN(_bcm_xgs5_ifa_msg_send_receive(unit, MOS_MSG_SUBCLASS_IFA_STAT_SET,
                                                       buffer_len, 0,
                                                       MOS_MSG_SUBCLASS_IFA_STAT_SET_REPLY, &reply_len,
                                                       SHR_IFA_MAX_UC_MSG_TIMEOUT));
    return BCM_E_NONE;
}
#else
typedef int bcm_make_iso_compilers_happy;
#endif /* INCLUDE_IFA */
