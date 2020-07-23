/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ipmc.c
 * Purpose: 
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_IPMC

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <bcm_int/common/debug.h>

#include <soc/defs.h>
 
#ifdef INCLUDE_L3
 
#include <soc/drv.h>
 
#include <soc/enet.h>
 
#include <shared/gport.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/ipmc.h>
#include <bcm/tunnel.h>
#include <bcm/stack.h>
#include <bcm/cosq.h>
#include <bcm/mpls.h>
#include <bcm/trunk.h>
#include <bcm/pkt.h>

#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/ipmc.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/trunk.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dpp/switch.h>


#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>

#include <soc/dcmn/dcmn_wb.h>

#ifdef BCM_ARAD_SUPPORT
#if defined(INCLUDE_KBP)
#include <soc/dpp/ARAD/arad_kbp.h>
#endif /* defined(INCLUDE_KBP) */
#endif /* BCM_ARAD_SUPPORT */


/***************************************************************/
/***************************************************************/

/*
 * Local defines
 */


#define DPP_IPMC_MSG(string)   "%s[%d]: " string, __FILE__, __LINE__


#define IPMC_SUPPORTED_FLAGS(unit) (IPMC_SUPPORTED_FLAGS_ARAD)


#define IPMC_SUPPORTED_FLAGS_PB   (BCM_IPMC_SOURCE_PORT_NOCHECK    | \
                                 BCM_IPMC_REPLACE                | \
                                 BCM_IPMC_IP6)

#define IPMC_SUPPORTED_FLAGS_ARAD   (BCM_IPMC_SOURCE_PORT_NOCHECK| \
                                 BCM_IPMC_REPLACE                | \
                                 BCM_IPMC_IP6                    | \
                                 BCM_IPMC_HIT_CLEAR              | \
                                 BCM_IPMC_HIT                    | \
                                 BCM_IPMC_RAW_ENTRY              | \
                                 BCM_IPMC_L2)

#define IPMC_CONFIG_SUPPORTED_FLAGS            (BCM_IPMC_REPLACE)
#define IPMC_CONFIG_SUPPORTED_TRAVERSE_FLAGS   (BCM_IPMC_HIT_CLEAR)

#define IPMC_ACCESS                 sw_state_access[unit].dpp.bcm.ipmc
#define DPP_IPMC_UNIT_VALID_CHECK \
    do {                                                                   \
        int _init;                                                         \
        uint8 _is_allocated;                                               \
        BCM_DPP_UNIT_CHECK(unit);                                          \
        if (!(SOC_DPP_CONFIG(unit)->pp.ipmc_enable)) return BCM_E_UNAVAIL; \
                                                                           \
        BCMDNX_IF_ERR_EXIT(IPMC_ACCESS.is_allocated(unit, &_is_allocated));\
        if(!_is_allocated) {                                               \
            return BCM_E_INIT;                                             \
        }                                                                  \
        BCMDNX_IF_ERR_EXIT(IPMC_ACCESS.init.get(unit, &_init));            \
        if (_init == FALSE) return BCM_E_INIT;                             \
        if (_init != TRUE) return _init;                                   \
    } while (0);
    


#define DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES      (100)
#define DPP_FRWRD_IP_ENTRIES_TO_SCAN(unit)      SOC_DPP_DEFS_GET_NOF_ENTRY_IDS(unit)
#define SOC_SAND_FALSE 0

#define DPP_FRWRD_MACT_GET_BLOCK_NOF_ENTRIES      (130)


/* 
 * Related Defines
 */

 
/***************************************************************/
/***************************************************************/

/***** Internal functions ******/

STATIC int 
bcm_petra_ipmc_entry_rp_add(
    int unit, 
    bcm_ipmc_addr_t *entry);

STATIC int 
bcm_petra_ipmc_entry_rp_remove(
    int unit, 
    bcm_ipmc_addr_t *entry,
    uint8 *is_bidir);

STATIC int 
bcm_petra_ipmc_entry_rp_get(
    int unit, 
    bcm_ipmc_addr_t *entry,
    uint8 *is_bidir);


/* Source port check */
int
  _bcm_petra_source_port_check(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    BCMDNX_INIT_FUNC_DEFS;
    if (data->ts) { /* Trunk port check */
        if (!BCM_DPP_TRUNK_VALID(data->port_tgid)) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "port tgid is invalid\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("port tgid is invalid")));
        }
    } else {        /* Source port check */
        if (!SOC_PORT_VALID(unit, data->port_tgid)) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "Source port is invalid\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Source port is invalid")));
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Validate general ipmc structure */
int
  _bcm_petra_ipmc_addr_validate(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    BCMDNX_INIT_FUNC_DEFS;
    if (data->flags & ~IPMC_SUPPORTED_FLAGS(unit)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "flags %x is not supported\n"), data->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("one or more flags is not supported")));
    }

    if ( (data->flags & BCM_IPMC_L2) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_set_entry_type_by_rif", 0) != 0)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "BCM_IPMC_L2 is redundant when ipmc_set_entry_type_by_rif property is set\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("BCM_IPMC_L2 is redundant when ipmc_set_entry_type_by_rif property is set")));
    }
    if ( (data->flags & BCM_IPMC_RAW_ENTRY) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mc_use_tcam", 0) != 0)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "BCM_IPMC_RAW_ENTRY is redundant when l3_mc_use_tcam property is set\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("BCM_IPMC_RAW_ENTRY is redundant when l3_mc_use_tcam property is set")));
    }
    
    if (data->flags & BCM_IPMC_IP6) {
        if ((bcm_ip6_mask_length(data->mc_ip6_mask)) && (!BCM_IP6_MULTICAST(data->mc_ip6_addr))) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit, "Destination ipv6 address is not mc\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("IPV6 destination address is not MC")));
        }

        if (BCM_IP6_MULTICAST(data->s_ip6_addr)) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "Source ipv6 address is mc\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("IPV6 source address is MC")));
        }

        if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)
            && (data->s_ip_addr > ((1 << _SHR_L3_IP6_COMPRESSED_SIP_MAX_NETLEN) -1))) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "IPv6 compressed SIP is bigger than MAX value:%d\n"),
                                  ((1 << _SHR_L3_IP6_COMPRESSED_SIP_MAX_NETLEN) -1)));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                               (_BSL_BCM_MSG_NO_UNIT("IPv6 compressed SIP is bigger than MAX value:"),
                                ((1 << _SHR_L3_IP6_COMPRESSED_SIP_MAX_NETLEN) -1)));
        }
    } else {
        /* IPV4 */
        if ((bcm_ip_mask_length(data->mc_ip_mask)) && (!BCM_IP4_MULTICAST(data->mc_ip_addr))) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "Destination ipv4 address is not mc\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("IPV4 destination address is not MC")));
        } 
        if (BCM_IP4_MULTICAST(data->s_ip_addr)) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "Source ipv4 address is mc\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("IPV4 source address is MC")));
        }
    }

#if !(defined(INCLUDE_KBP))
    /* IPMC module does not support vrf != 0 in devices without KAPS*/
    if (!_BCM_DPP_IPMC_BIDIR_SUPPORTED(unit) && data->vrf != 0) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Invalid vrf, IPMC module only supports vrf = 0\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("invalid vrf, IPMC module only supports vrf = 0")));
    }
#endif

    
#if 0
defined(INCLUDE_KBP)
    if (((ARAD_KBP_ENABLE_IPV6_MC && data->flags & BCM_IPMC_IP6) || (ARAD_KBP_ENABLE_IPV4_MC && (data->flags & ~BCM_IPMC_IP6))) 
        && (data->flags & BCM_IPMC_RAW_ENTRY))
    {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Flag BCM_IPMC_RAW_ENTRY is not supported on external memory")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("Flag BCM_IPMC_RAW_ENTRY is not supported on external memory.")));
    }
    else 
#endif
    if (!SOC_IS_JERICHO(unit) && (data->flags & BCM_IPMC_RAW_ENTRY))
    {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Flag BCM_IPMC_RAW_ENTRY supported on Jericho and above devices")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("Flag BCM_IPMC_RAW_ENTRY supported on devices Jericho and above devices")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Find internal functions */
int
  _bcm_ppd_frwrd_ipv4_mc_route_find(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv,
        rv = BCM_E_NONE;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO
        route_info;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        route_status=0;
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
        location;
    uint8
        get_flags=0,
        found,
        is_bidir=0;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    /* do operation on BIDIR DB, if exist done otherwise continue  */
    if (data->rp_id != BCM_IPMC_RP_ID_INVALID) {
        rv = bcm_petra_ipmc_entry_rp_get(unit,data,&is_bidir);
    }
    if (is_bidir) {
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }

    data->rp_id = BCM_IPMC_RP_ID_INVALID;

    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(&route_info);

    /* Route key */
    route_key.group             = data->mc_ip_addr;
    route_key.group_prefix_len  = bcm_ip_mask_length(data->mc_ip_mask);
    route_key.inrif_valid       = data->vid != 0;
    route_key.inrif             = data->vid;
    route_key.source.ip_address = data->s_ip_addr;
    route_key.vrf_ndx           = data->vrf;
    /* Source IP address 0x0 means no lookup for SIP */
    if (data->s_ip_addr == 0x0) {
        route_key.source.prefix_len = 0;
    } else {
        route_key.source.prefix_len = bcm_ip_mask_length(data->s_ip_mask);
    }    

    get_flags = SOC_PPC_FRWRD_IP_EXACT_MATCH;

    if (data->flags & BCM_IPMC_HIT_CLEAR) {
        get_flags |= SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    if (data->flags & BCM_IPMC_RAW_ENTRY) {
        route_info.flags = _BCM_IPMC_FLAGS_RAW_ENTRY;
    }
    soc_sand_rv = soc_ppd_frwrd_ipv4_mc_route_get(unit,&route_key,get_flags,&route_info,&route_status,&location,&found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (found == SOC_SAND_FALSE) {
        /* Entry not found */
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    } 
    
    /* Entry found */
    if (route_status & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
        data->flags |= BCM_IPMC_HIT;
    }

    if (route_info.dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST) {
        _BCM_MULTICAST_GROUP_SET(data->group, _BCM_MULTICAST_TYPE_L3, route_info.dest_id.dest_val);
    } else if (route_info.dest_id.dest_type == SOC_SAND_PP_DEST_FEC) {
        BCM_L3_ITF_SET(data->l3a_intf, BCM_L3_ITF_TYPE_FEC, route_info.dest_id.dest_val);
    } else if ((data->flags & BCM_IPMC_RAW_ENTRY) ){
        /* in case of raw data entry no encoding in needed*/
        data->l3a_intf = route_info.dest_id.dest_val;
    } else {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "entry get ipv4 resolve dest type differ from multicast\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("in IPV4 entry resolve dest-type differ from multicast")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_route_key(
    int unit,
    bcm_ipmc_addr_t *data,
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY *route_key
  )
{
    bcm_error_t
        rv;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(route_key);

    /* Route key */ 
    route_key->inrif.val         = data->vid;
    route_key->inrif.mask        = (route_key->inrif.val > 0) ? (SOC_DPP_CONFIG(unit)->l3.nof_rifs) - 1 : 0;
    route_key->vrf_ndx           = data->vrf;
    rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, data->mc_ip6_addr, &(route_key->group.ipv6_address));
    BCMDNX_IF_ERR_EXIT(rv);
    route_key->group.prefix_len = bcm_ip6_mask_length(data->mc_ip6_mask);

    /* Source IP address 0x0 means no lookup for SIP. In Arad, SIP in MC lookup only for external TCAM */
#ifdef BCM_ARAD_SUPPORT
#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_MC) 
    {
        if (ARAD_KBP_IPV6_LOOKUP_MODEL_1_IS_IN_USE(unit)) {
            rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, data->s_ip6_addr, &(route_key->source.ipv6_address));
            BCMDNX_IF_ERR_EXIT(rv);
            route_key->source.prefix_len = bcm_ip6_mask_length(data->s_ip6_mask);
        } else {
            bcm_ip6_t zero_addr;
            sal_memset(&zero_addr, 0x0, sizeof(bcm_ip6_t));
            if (sal_memcmp(data->s_ip6_addr, &zero_addr, sizeof(bcm_ip6_t)) == 0) {
                route_key->source.prefix_len = 0;
            } else {
                route_key->source.prefix_len = _SHR_L3_IP6_MAX_NETLEN;
                rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, data->s_ip6_addr, &(route_key->source.ipv6_address));
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    }
    else
#endif /* defined(INCLUDE_KBP) */
#endif /* BCM_ARAD_SUPPORT */
    {
        route_key->source.prefix_len = 0;

        /* IPv6 compressed SIP */
        if (SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit) && (data->s_ip_addr != 0)) {
            route_key->compressed_sip = data->s_ip_addr;
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
  _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_compress_sip_key(
    int unit,
    bcm_ipmc_addr_t *data,
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY *route_key
  )
{
    bcm_error_t rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(route_key);

    /* Route key */
    route_key->inrif.val         = data->vid;
    route_key->inrif.mask        = (route_key->inrif.val > 0) ? (SOC_DPP_CONFIG(unit)->l3.nof_rifs - 1) : 0;
    route_key->source.prefix_len = bcm_ip6_mask_length(data->s_ip6_mask);
    rv = _bcm_l3_bcm_ipv6_addr_to_sand_ipv6_addr(unit, data->s_ip6_addr, &(route_key->source.ipv6_address));
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_route_find(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv;
    bcm_error_t
        rv;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO
        route_info;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        route_status;
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
        location;
    uint8
        get_flags=0,
        found;

    BCMDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_MC && ARAD_KBP_IPV6MC_INVERT_GROUP_SOURCE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Special case of IPv6 MC look-up in KBP is in use. bcm_ipmc_find is disabled for IPv6 entries")));
    }
#endif

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipv6_mc_forwarding_disable", 0))
    {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("ipV6mc APIs is unavailable when property ipv6_mc_forwarding_disable=1")));    
    }

    unit = (unit);

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&route_info);

    /* Route key */ 
    rv = _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_route_key(unit, data, &route_key);
    BCMDNX_IF_ERR_EXIT(rv);

    get_flags = SOC_PPC_FRWRD_IP_EXACT_MATCH;

    if (data->flags & BCM_IPMC_HIT_CLEAR) {
        get_flags |= SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    if (data->flags & BCM_IPMC_RAW_ENTRY) {
        route_info.flags = _BCM_IPMC_FLAGS_RAW_ENTRY;
    }

    soc_sand_rv = soc_ppd_frwrd_ipv6_mc_route_get(unit,&route_key,get_flags,&route_info,&route_status,&location,&found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (found == SOC_SAND_FALSE) {
        /* Entry not found */
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }
    
    /* Entry found */
    if (route_status & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
        data->flags |= BCM_IPMC_HIT;
    }

    if (route_info.dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST) {
        _BCM_MULTICAST_GROUP_SET(data->group,_BCM_MULTICAST_TYPE_L3,route_info.dest_id.dest_val);
    }
    else if (route_info.dest_id.dest_type == SOC_SAND_PP_DEST_FEC) {
        BCM_L3_ITF_SET(data->l3a_intf, BCM_L3_ITF_TYPE_FEC, route_info.dest_id.dest_val); 
    } else if (data->flags & BCM_IPMC_RAW_ENTRY){
        data->l3a_intf = route_info.dest_id.dest_val;
    }else {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "entry get ipv6 resolve dest type differ from multicast\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("in IPV6 entry resolve dest-type differ from multicast")));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_compress_sip_find(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv = SOC_SAND_OK;
    bcm_error_t
        rv = BCM_E_NONE;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO
        route_info;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        route_status;
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
        location;
    uint8
        get_flags = SOC_PPC_FRWRD_IP_EXACT_MATCH;
    uint8
        found = SOC_SAND_FALSE;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&route_info);

    /* Route key */
    rv = _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_compress_sip_key(unit, data, &route_key);
    BCMDNX_IF_ERR_EXIT(rv);

    if (data->flags & BCM_IPMC_HIT_CLEAR) {
        get_flags |= SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    route_info.flags = _BCM_IPMC_FLAGS_RAW_ENTRY;
    soc_sand_rv = soc_ppd_frwrd_ipv6_mc_compress_sip_get(unit, &route_key, get_flags, &route_info,
                                                      &route_status, &location, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (found == SOC_SAND_FALSE) {
        /* Entry not found */
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    /* Entry found */
    if (route_status & SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED) {
        data->flags |= BCM_IPMC_HIT;
    }

    data->s_ip_addr = route_info.dest_id.dest_val;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* check if the entry to be added to routing DB or MACT
 * depends if routing is enable or the in_rif
 */

STATIC
int
  _bcm_ppd_frwrd_ipv4_mc_is_bridged(
    int unit,
    bcm_ipmc_addr_t *data,
    uint8 *bridge
  )
{
    uint32
        soc_sand_rv;
    uint32
        vsi;
    SOC_PPC_RIF_INFO
      rif_info;

    BCMDNX_INIT_FUNC_DEFS;

    *bridge = 0;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_set_entry_type_by_rif", 0) == 1) {

        vsi = data->vid;

        /* vsi not specified then this is entry for routing */
        if (vsi == 0) {
            BCM_EXIT;
        }

        /* get rif attributes */
        soc_sand_rv = soc_ppd_rif_vsid_map_get(
                  unit,
                  vsi,
                  &rif_info
              );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* if routing is disable on */
        if (!(rif_info.routing_enablers_bm & SOC_PPC_RIF_ROUTE_ENABLE_TYPE_IPV4_MC)) {
            *bridge = 1;
        }

    } else if (data->flags & BCM_IPMC_L2) {
        *bridge = 1;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC
int
  _bcm_ppd_frwrd_ipv4_mc_bridge_add(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv;
    bcm_error_t
        rv;
    SOC_PPC_FRWRD_MACT_ENTRY_KEY
        route_key;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE
        route_info;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY ssm_route_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO ssm_route_info;
    SOC_SAND_SUCCESS_FAILURE
        failure_indication = SOC_SAND_SUCCESS;
    bcm_ipmc_addr_t data_find;
    uint8 ipmc_ssm_mode;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    ipmc_ssm_mode = SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode;

    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&route_key);
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&route_info);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&ssm_route_key);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(&ssm_route_info);
    bcm_ipmc_addr_t_init(&data_find);

    sal_memcpy(&data_find,data,sizeof(bcm_ipmc_addr_t));

    rv = bcm_petra_ipmc_find(unit,&data_find);
    if (rv == BCM_E_NONE && (!(data->flags & BCM_IPMC_REPLACE))) {
       LOG_ERROR(BSL_LS_BCM_IPMC,
                 (BSL_META_U(unit,
                             "entry key given already exists and REPLACE flag is disabled\n")));
       BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("entry key given already exists and REPLACE flag is disabled")));
    }
    if (rv == BCM_E_NOT_FOUND && ((data->flags & BCM_IPMC_REPLACE))) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                (BSL_META_U(unit,
                        "entry key given doesnt exist and REPLACE flag is enabled\n")));
    }
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (!(_BCM_MULTICAST_IS_SET(data->group) && _BCM_MULTICAST_IS_L3(data->group)) &&
        !((data->group == BCM_MULTICAST_INVALID) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)))) {
        /* Multicast ID is invalid */
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "ipmc index 0x%08x is invalid\n"), data->group));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ipmc index is invalid")));  
    }

    if (data->s_ip_addr == 0) {
        /* Route key */
        route_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;
        route_key.key_val.ipv4_mc.dip = data->mc_ip_addr;
        route_key.key_val.ipv4_mc.fid = data->vid;

        /* Route info */
        /* Resolve ipmc index */
        if ((data->group == BCM_MULTICAST_INVALID) && (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0))){
            SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &route_info.frwrd_info.forward_decision, soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else if (_BCM_MULTICAST_IS_SET(data->group)) {
            SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit,&route_info.frwrd_info.forward_decision,_BCM_MULTICAST_ID_GET(data->group), soc_sand_rv);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        soc_sand_rv = soc_ppd_frwrd_mact_entry_add(unit,SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT,&route_key,&route_info,&failure_indication);
        SOC_SAND_IF_FAIL_RETURN(failure_indication);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else { /* SIP != 0 */
         if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {

            route_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC_SSM;
            route_key.key_val.ipv4_mc.dip = data->mc_ip_addr & 0xffffff;
            route_key.key_val.ipv4_mc.sip = data->s_ip_addr;
            route_key.key_val.ipv4_mc.fid = data->vid;

            /* Route info */
            /* Resolve ipmc index */
           if (data->group == BCM_MULTICAST_INVALID) {
                SOC_PPD_FRWRD_DECISION_DROP_SET(unit, &route_info.frwrd_info.forward_decision, soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else if (_BCM_MULTICAST_IS_SET(data->group)) {
                SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(unit,&route_info.frwrd_info.forward_decision,_BCM_MULTICAST_ID_GET(data->group), soc_sand_rv);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           }
            soc_sand_rv = soc_ppd_frwrd_mact_entry_add(unit,SOC_PPC_FRWRD_MACT_ADD_TYPE_INSERT,&route_key,&route_info,&failure_indication);
            SOC_SAND_IF_FAIL_RETURN(failure_indication);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

         } else if ((ipmc_ssm_mode != BCM_IPMC_SSM_DISABLE) || (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl && data->lookup_class)  ) {

            ssm_route_key.fid = data->vid;
            ssm_route_key.group = data->mc_ip_addr;
            ssm_route_key.group_prefix_len = bcm_ip_mask_length(data->mc_ip_mask);
            ssm_route_key.source.ip_address = data->s_ip_addr;
            ssm_route_key.source.prefix_len = bcm_ip_mask_length(data->s_ip_mask);

            if (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
                ssm_route_key.with_vlan_lkup = data->lookup_class;
                ssm_route_key.vlan_tag = data->distribution_class;
            } else {
                ssm_route_key.with_vlan_lkup = 0x0;
            }

            ssm_route_info.dest_id.dest_type    = SOC_SAND_PP_DEST_MULTICAST;
            ssm_route_info.dest_id.dest_val     = _BCM_MULTICAST_ID_GET(data->group);

            soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_ipmc_ssm_entry_add,(unit, &ssm_route_key, &ssm_route_info, &failure_indication)));

            SOC_SAND_IF_FAIL_RETURN(failure_indication);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        } else { /* IPMC SSM disabled */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("when add IPMC entry as bridged SIP has to be seto to zero")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC
int
  _bcm_ppd_frwrd_ipv4_mc_bridged_find(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv;
    SOC_PPC_FRWRD_MACT_ENTRY_KEY
        route_key;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE
        route_info;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY ssm_route_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO ssm_route_info;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS    route_status;
    uint8
        found = 0;
    uint8 ipmc_ssm_mode;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    ipmc_ssm_mode = SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode;;

    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&route_key);
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE_clear(&route_info);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&ssm_route_key);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(&ssm_route_info);

    if (data->s_ip_addr == 0) {
        /* Route key */
        route_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;
        route_key.key_val.ipv4_mc.dip = data->mc_ip_addr;
        route_key.key_val.ipv4_mc.fid = data->vid;

        soc_sand_rv = soc_ppd_frwrd_mact_entry_get(unit,&route_key,&route_info,&found);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else { /* SIP != 0 */
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
            route_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC_SSM;
            route_key.key_val.ipv4_mc.sip = data->s_ip_addr;
            route_key.key_val.ipv4_mc.dip = data->mc_ip_addr/* & 0xffffff*/;
            route_key.key_val.ipv4_mc.fid = data->vid;

            soc_sand_rv = soc_ppd_frwrd_mact_entry_get(unit,&route_key,&route_info,&found);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else if ((ipmc_ssm_mode != BCM_IPMC_SSM_DISABLE) || (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl && data->lookup_class)  ) {
            /* build TCAM routing key */
            ssm_route_key.fid = data->vid;
            ssm_route_key.group = data->mc_ip_addr;
            ssm_route_key.group_prefix_len = bcm_ip_mask_length(data->mc_ip_mask);
            ssm_route_key.source.ip_address = data->s_ip_addr;
            ssm_route_key.source.prefix_len = bcm_ip_mask_length(data->s_ip_mask);

            if (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
                ssm_route_key.with_vlan_lkup = data->lookup_class;
                ssm_route_key.vlan_tag = data->distribution_class;
            } else {
                ssm_route_key.with_vlan_lkup = 0x0;
            }
            soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_ipmc_ssm_entry_get,(unit, &ssm_route_key, &ssm_route_info, &route_status, &found)));          
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else { /* IPMC SSM disabled */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("when add IPMC entry as bridged SIP has to be seto to zero")));
        }
    }
    if (!found) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    if (route_info.frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_MC) {
        _BCM_MULTICAST_GROUP_SET(data->group,_BCM_MULTICAST_TYPE_L3,route_info.frwrd_info.forward_decision.dest_id);
    }
    else if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0) &&
             route_info.frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_DROP) {
        data->group = BCM_MULTICAST_INVALID;
    }
    else if (ssm_route_info.dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST) {
        _BCM_MULTICAST_GROUP_SET(data->group,_BCM_MULTICAST_TYPE_L3,ssm_route_info.dest_id.dest_val);
    }
    else{
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("not expected destination")));    
    }

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC
int
  _bcm_ppd_frwrd_ipv4_mc_bridge_remove(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32                           soc_sand_rv;
    bcm_error_t                      rv;
    SOC_PPC_FRWRD_MACT_ENTRY_KEY     route_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY  ssm_route_key;
    bcm_ipmc_addr_t                  data_find;
    uint8                            ipmc_ssm_mode;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    ipmc_ssm_mode = SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode;;

    SOC_PPC_FRWRD_MACT_ENTRY_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&ssm_route_key);
    bcm_ipmc_addr_t_init(&data_find);

    sal_memcpy(&data_find,data,sizeof(bcm_ipmc_addr_t));

    rv = bcm_petra_ipmc_find(unit,&data_find);

    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    if (rv == BCM_E_NOT_FOUND) {
       LOG_INFO(BSL_LS_BCM_IPMC, (BSL_META_U(unit, "entry key given does not exist\n")));
       BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);       
    }

    if (data->s_ip_addr == 0) {
        /* Route key */
        route_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;
        route_key.key_val.ipv4_mc.dip = data->mc_ip_addr;
        route_key.key_val.ipv4_mc.fid = data->vid;

        soc_sand_rv = soc_ppd_frwrd_mact_entry_remove(unit,&route_key, 0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else { /* SIP != 0 */
        if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipmc_l2_ssm_mode_lem", 0)) {
            route_key.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC_SSM;
            route_key.key_val.ipv4_mc.sip = data->s_ip_addr;
            route_key.key_val.ipv4_mc.dip = data->mc_ip_addr & 0xffffff;
            route_key.key_val.ipv4_mc.fid = data->vid;

            soc_sand_rv = soc_ppd_frwrd_mact_entry_remove(unit,&route_key, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else if ((ipmc_ssm_mode != BCM_IPMC_SSM_DISABLE) || (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl && data->lookup_class)  ) {
            /* build TCAM routing key */
            ssm_route_key.fid = data->vid;
            ssm_route_key.group = data->mc_ip_addr;
            ssm_route_key.group_prefix_len = bcm_ip_mask_length(data->mc_ip_mask);
            ssm_route_key.source.ip_address = data->s_ip_addr;
            ssm_route_key.source.prefix_len = bcm_ip_mask_length(data->s_ip_mask);

            if (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl) {
                ssm_route_key.with_vlan_lkup = data->lookup_class;
                ssm_route_key.vlan_tag = data->distribution_class;
            } else {
                ssm_route_key.with_vlan_lkup = 0x0;
            }
            soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_ipmc_ssm_entry_delete,(unit, &ssm_route_key)));            
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
        else { /* IPMC SSM disabled */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("when add IPMC entry as bridged SIP has to be set to zero")));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_mc_bridge_clear(
    int unit
  )
{
    uint32
        soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    if(SOC_IS_ARADPLUS(unit)) {
        soc_sand_rv = soc_ppd_frwrd_mact_ipmc_table_clear(unit);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    if((SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode != BCM_IPMC_SSM_DISABLE) || (SOC_DPP_CONFIG(unit)->pp.ipmc_ivl)) {
        soc_sand_rv = (MBCM_PP_DRIVER_CALL(unit,mbcm_pp_ipmc_ssm_entry_clear,(unit)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int
  _bcm_ppd_frwrd_ipv4_mc_ssm_get_block(
    int unit, 
    uint32 flags, 
    bcm_ipmc_traverse_cb cb, 
    void *user_data
  )
{
    uint32
        soc_sand_rv,
        nof_entries = 0;
    SOC_PPC_IP_ROUTING_TABLE_RANGE
        block_range_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY
        *route_keys = NULL;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO
        routes_info[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
        routes_location[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    bcm_ipmc_addr_t 
        entry;
    int idx;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range_key);

    /* Block range key */
    block_range_key.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED; /* type is not used, set only to pass verify */
    block_range_key.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range_key.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN(unit);
    BCMDNX_ALLOC(route_keys, sizeof(SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_mc_ssm_get_block.route_keys");
    if (route_keys == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
 
    while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&block_range_key.start.payload))
    {
      nof_entries = 0;
      /* Call function */
      soc_sand_rv = soc_ppd_frwrd_ipv4_mc_ssm_get_block(unit,&block_range_key,route_keys,routes_info,routes_status,routes_location,&nof_entries);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      
      for(idx = 0; idx < nof_entries; idx++)
      {
        bcm_ipmc_addr_t_init(&entry);
        /* Valid entries,            */
        /* Fill MC entry infromation */
        entry.vid = route_keys[idx].fid;
        entry.mc_ip_addr    = route_keys[idx].group;
        entry.mc_ip_mask    = bcm_ip_mask_create(route_keys[idx].group_prefix_len);
        entry.s_ip_addr     = route_keys[idx].source.ip_address;
        entry.s_ip_mask     = bcm_ip_mask_create(route_keys[idx].source.prefix_len);
        
        if (routes_info[idx].dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST) {
            entry.group = routes_info[idx].dest_id.dest_val;
        } else {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "entry get block ipv4 resolve dest type differ from multicast\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("in IPV4 entry resolved dest-type differ from multicast")));
        }
        /* Invoke user callback. */
        (*cb)(unit, &entry, user_data);
        
       }
    }

    BCM_EXIT;
exit:
    BCM_FREE(route_keys);
    BCMDNX_FUNC_RETURN;
}


int
  _bcm_ppd_frwrd_ipv4_mc_bridge_get_block(
    int unit, 
    uint32 flags, 
    bcm_ipmc_traverse_cb cb, 
    void *user_data
  )
{
    uint32
        soc_sand_rv,
        nof_entries = 0;
    SOC_SAND_TABLE_BLOCK_RANGE                      block_range;
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE          rule;
    SOC_PPC_FRWRD_MACT_ENTRY_KEY    *l2_traverse_mact_keys = NULL;
    SOC_PPC_FRWRD_MACT_ENTRY_VALUE  *l2_traverse_mact_vals = NULL;
    bcm_ipmc_addr_t entry;
    int idx;
    uint8 ipmc_ssm_mode;

    BCMDNX_INIT_FUNC_DEFS;

    ipmc_ssm_mode = SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode;

    BCMDNX_ALLOC(l2_traverse_mact_keys, sizeof(SOC_PPC_FRWRD_MACT_ENTRY_KEY)*DPP_FRWRD_MACT_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_mc_bridge_get_block.mact_keys");
    if (l2_traverse_mact_keys == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
    
    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    SOC_PPC_FRWRD_MACT_TRAVERSE_MATCH_RULE_clear(&rule);

    SOC_SAND_TBL_ITER_SET_BEGIN(&(block_range.iter));
    block_range.entries_to_act = DPP_FRWRD_MACT_GET_BLOCK_NOF_ENTRIES;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    rule.key_type = SOC_PPC_FRWRD_MACT_KEY_TYPE_IPV4_MC;

    if (!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        BCMDNX_ALLOC(l2_traverse_mact_vals, sizeof(*l2_traverse_mact_vals)*DPP_FRWRD_MACT_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_mc_bridge_get_block.l2_traverse_mact_vals");
        if (l2_traverse_mact_vals == NULL) {        
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
        }
    }
    while(!SOC_SAND_TBL_ITER_IS_END(&(block_range.iter))) {
        /* activate traverse for MACT */
        soc_sand_rv = soc_ppd_frwrd_mact_get_block(unit,
                                           &rule,
                                           SOC_PPC_FRWRD_MACT_TABLE_SW_HW,
                                           &block_range,
                                           l2_traverse_mact_keys,
                                           l2_traverse_mact_vals,
                                           &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        if(nof_entries == 0) {
            break;
        }
        for (idx = 0; idx < nof_entries; ++idx) {
            bcm_ipmc_addr_t_init(&entry);

            entry.mc_ip_addr = l2_traverse_mact_keys[idx].key_val.ipv4_mc.dip;
            entry.vid = l2_traverse_mact_keys[idx].key_val.ipv4_mc.fid;

            if (l2_traverse_mact_vals[idx].frwrd_info.forward_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_MC) {
                _BCM_MULTICAST_GROUP_SET(entry.group,_BCM_MULTICAST_TYPE_L3,l2_traverse_mact_vals[idx].frwrd_info.forward_decision.dest_id);
            }

            /* Invoke user callback. */
            (*cb)(unit, &entry, user_data);
        }
    }

    if (ipmc_ssm_mode != BCM_IPMC_SSM_DISABLE) {
        _bcm_ppd_frwrd_ipv4_mc_ssm_get_block(unit, flags, cb, user_data);
    }

    BCM_EXIT;
exit:
    BCM_FREE(l2_traverse_mact_keys);
    BCM_FREE(l2_traverse_mact_vals);
    BCMDNX_FUNC_RETURN;
}


/* Add internal funcions */
/* Creates a new ipv4 mc route entry */
int
  _bcm_ppd_frwrd_ipv4_mc_route_add(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv;
    bcm_error_t
        rv;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO
        route_info;
    SOC_SAND_SUCCESS_FAILURE
        success;
    bcm_ipmc_addr_t data_find;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);


    /* this is actually just RP entry addition */
    if (data->rp_id != BCM_IPMC_RP_ID_INVALID) {
        return bcm_petra_ipmc_entry_rp_add(unit,data);
    }

    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(&route_info);
    bcm_ipmc_addr_t_init(&data_find);

    sal_memcpy(&data_find,data,sizeof(bcm_ipmc_addr_t));

    rv = bcm_petra_ipmc_find(unit,&data_find);
        
    if (rv == BCM_E_NONE && (!(data->flags & BCM_IPMC_REPLACE))) {
       LOG_ERROR(BSL_LS_BCM_IPMC,
                 (BSL_META_U(unit,
                             "entry key given already exists and REPLACE flag is disabled\n")));
       BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("entry key given already exists and REPLACE flag is disabled")));
    }
    if (rv == BCM_E_NOT_FOUND && ((data->flags & BCM_IPMC_REPLACE))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("entry key given does not exist and REPLACE flag is enabled")));
    }
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Route key */
    route_key.group             = data->mc_ip_addr;
    route_key.group_prefix_len  = bcm_ip_mask_length(data->mc_ip_mask);
    route_key.inrif_valid       = data->vid != 0;
    route_key.inrif             = data->vid;
    route_key.source.ip_address = data->s_ip_addr;
    route_key.vrf_ndx           = data->vrf;
    /* Source IP address 0x0 means no lookup for SIP */
    if (data->s_ip_addr == 0x0) {
        route_key.source.prefix_len = 0;
    } else {
        route_key.source.prefix_len = bcm_ip_mask_length(data->s_ip_mask);
    }   
    
    /* Route info */

    /* Resolve ipmc index */    
    if (_BCM_MULTICAST_IS_SET(data->group) && !(data->flags & BCM_IPMC_RAW_ENTRY)) {
        if (_BCM_MULTICAST_IS_L3(data->group)) {
            route_info.dest_id.dest_type    = SOC_SAND_PP_DEST_MULTICAST;
            route_info.dest_id.dest_val     = _BCM_MULTICAST_ID_GET(data->group);

            /* The given ipmc_index was created by bcm_multicast APIs. It's
             * assumed that the IPMC group's l2_pbmp and l3_pbmp have already
             * been or will be configured by bcm_multicast APIs.
             */
        } else {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "ipmc index is not a L3 group\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group is not a L3 MC group")));    
        }
    } 
    else if (BCM_L3_ITF_TYPE_IS_FEC(data->l3a_intf)){
             route_info.dest_id.dest_type = SOC_SAND_PP_DEST_FEC;
             route_info.dest_id.dest_val = BCM_L3_ITF_VAL_GET(data->l3a_intf); 
    /* in case of raw data payload no encoding is needed*/
    } else if ((data->l3a_intf) && (data->flags & BCM_IPMC_RAW_ENTRY)) {
            route_info.dest_id.dest_type = SOC_SAND_PP_DEST_FEC;
            route_info.dest_id.dest_val = data->l3a_intf;
            route_info.flags |= _BCM_IPMC_FLAGS_RAW_ENTRY;
    } else {
        /* Multicast ID is invalid */
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "ipmc index is invalid\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ipmc index is invalid")));    
    }
    
        
    /* Add / Update entry */
    soc_sand_rv = soc_ppd_frwrd_ipv4_mc_route_add(unit,&route_key,&route_info,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (success != SOC_SAND_SUCCESS) {
        /* Out of resources or other internal error */
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Table is out of resources\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("out of resources")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_route_add(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
      soc_sand_rv;
    bcm_error_t
      rv;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO
        route_info;
    SOC_SAND_SUCCESS_FAILURE
        success;
    bcm_ipmc_addr_t
        data_find;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_MC && ARAD_KBP_IPV6MC_INVERT_GROUP_SOURCE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Special case of IPv6 MC look-up in KBP is in use. bcm_ipmc_add is disabled for IPv6 entries")));
    }
#endif

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipv6_mc_forwarding_disable", 0))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("ipmc APIs is unavailable when property ipv6_mc_forwarding_disable=1")));    
    }

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&route_info);
    bcm_ipmc_addr_t_init(&data_find);

    sal_memcpy(&data_find,data,sizeof(bcm_ipmc_addr_t));

    /* Check if entry is already existed */
    rv = bcm_petra_ipmc_find(unit,&data_find);

    if (rv == BCM_E_NONE && (!(data->flags & BCM_IPMC_REPLACE))) {
       LOG_ERROR(BSL_LS_BCM_IPMC,
                 (BSL_META_U(unit,
                             "entry key given already exists and REPLACE flag is disabled\n")));
       BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("entry key given already exists and REPLACE flag is disabled")));
    }
    if (rv == BCM_E_NOT_FOUND && ((data->flags & BCM_IPMC_REPLACE))) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                (BSL_META_U(unit,
                        "entry key given doesn't exist and REPLACE flag is enabled\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("entry key given doesn't exist and REPLACE flag is enabled")));
    }
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Route key */
    rv = _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_route_key(unit, data, &route_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Route info */

    /* Resolve ipmc index */
     if (_BCM_MULTICAST_IS_SET(data->group) && !(data->flags & BCM_IPMC_RAW_ENTRY)) {
        if (_BCM_MULTICAST_IS_L3(data->group)) {
            route_info.dest_id.dest_type    = SOC_SAND_PP_DEST_MULTICAST;
            route_info.dest_id.dest_val     = _BCM_MULTICAST_ID_GET(data->group);
            /*
             * The given ipmc_index was created by bcm_multicast APIs. It's
             * assumed that the IPMC group's l2_pbmp and l3_pbmp have already
             * been or will be configured by bcm_multicast APIs.
             */
        } else {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "ipmc index is not a L3 group \n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ipmc index is not a L3 MC group")));
        }
    } else if (BCM_L3_ITF_TYPE_IS_FEC(data->l3a_intf) && (SOC_IS_JERICHO(unit))) {
        route_info.dest_id.dest_type = SOC_SAND_PP_DEST_FEC;
        route_info.dest_id.dest_val = BCM_L3_ITF_VAL_GET(data->l3a_intf);
    } else if ((data->flags & BCM_IPMC_RAW_ENTRY) &&  (data->l3a_intf)) {
        /* in case of raw data payload no encoding is needed */
        route_info.dest_id.dest_val = data->l3a_intf;
        route_info.flags |= _BCM_IPMC_FLAGS_RAW_ENTRY;
        route_info.dest_id.dest_type = SOC_SAND_PP_DEST_FEC;
    } else {
        /* Multicast ID is invalid */
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "ipmc index is invalid\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ipmc index is invalid")));
    }


    /* Add / Update entry */
    soc_sand_rv = soc_ppd_frwrd_ipv6_mc_route_add(unit, &route_key, &route_info, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

    if (success != SOC_SAND_SUCCESS) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Internal error %s\n"),soc_sand_SAND_SUCCESS_FAILURE_to_string(success)));
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("out of resources")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_compress_sip_add(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv = SOC_SAND_OK;
    bcm_error_t
        rv = BCM_E_NONE;
    SOC_SAND_SUCCESS_FAILURE
        success = SOC_SAND_SUCCESS;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO
        route_info;
    bcm_ipmc_addr_t
        data_find;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&route_info);
    bcm_ipmc_addr_t_init(&data_find);

    /* Check if entry is already exist */
    sal_memcpy(&data_find, data, sizeof(bcm_ipmc_addr_t));
    rv = bcm_petra_ipmc_config_find(unit, &data_find);
    if (rv == BCM_E_NONE && (!(data->flags & BCM_IPMC_REPLACE))) {
       LOG_ERROR(BSL_LS_BCM_IPMC,
                 (BSL_META_U(unit,
                             " given entry key already exist and REPLACE flag is disabled\n")));
       BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS,
          (_BSL_BCM_MSG("given entry key already exist and REPLACE flag is disabled")));
    }
    if (rv == BCM_E_NOT_FOUND && ((data->flags & BCM_IPMC_REPLACE))) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                (BSL_META_U(unit, "given entry key doesn't exist and REPLACE flag is enabled\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
            (_BSL_BCM_MSG("given entry key doesn't exist and REPLACE flag is enabled")));
    }
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_BCM_IPMC, (BSL_META_U(unit, "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* Route key */
    rv = _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_compress_sip_key(unit, data, &route_key);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Route info */
    route_info.dest_id.dest_type = SOC_SAND_PP_DEST_FEC;
    /* in case of raw data payload no encoding is needed */
    route_info.flags |= _BCM_IPMC_FLAGS_RAW_ENTRY;
    route_info.dest_id.dest_val = data->s_ip_addr;

    /* Add or update entry */
    soc_sand_rv = soc_ppd_frwrd_ipv6_mc_compress_sip_add(unit, &route_key, &route_info, &success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (success != SOC_SAND_SUCCESS) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Internal error %s\n"),soc_sand_SAND_SUCCESS_FAILURE_to_string(success)));
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("out of resources")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Remove internal functions */
int
  _bcm_ppd_frwrd_ipv4_mc_route_remove(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv;
    bcm_error_t
        rv = BCM_E_NONE;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO
        route_info;
    SOC_SAND_SUCCESS_FAILURE
        success;
    bcm_ipmc_addr_t
        data_find;
    uint8
        is_bidir=0;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);
    success = SOC_SAND_FAILURE_UNKNOWN_ERR;

    /* do operation on BIDIR DB, if exist done otherwise continue  */
    if (data->rp_id != BCM_IPMC_RP_ID_INVALID) {
        rv = bcm_petra_ipmc_entry_rp_remove(unit,data,&is_bidir);
    }
    if (is_bidir) {
        BCMDNX_IF_ERR_EXIT(rv);
        BCM_EXIT;
    }


    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO_clear(&route_info);
    bcm_ipmc_addr_t_init(&data_find);

    sal_memcpy(&data_find,data,sizeof(bcm_ipmc_addr_t));

    /* check if exists before trying to remove */
    rv = bcm_petra_ipmc_find(unit,&data_find);
    
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }
        
    if (rv == BCM_E_NOT_FOUND) {
       LOG_INFO(BSL_LS_BCM_IPMC, (BSL_META_U(unit, "entry key given does not exist\n")));
       BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);      
    }

    /* Route key */
    route_key.group             = data->mc_ip_addr;
    route_key.group_prefix_len  = bcm_ip_mask_length(data->mc_ip_mask);
    route_key.inrif_valid       = data->vid != 0;
    route_key.inrif             = data->vid;
    route_key.source.ip_address = data->s_ip_addr;
    route_key.vrf_ndx           = data->vrf;
    /* Source IP address 0x0 means no lookup for SIP */
    if (data->s_ip_addr == 0x0) {
        route_key.source.prefix_len = 0;
    } else {
        route_key.source.prefix_len = bcm_ip_mask_length(data->s_ip_mask);
    }

    soc_sand_rv = soc_ppd_frwrd_ipv4_mc_route_remove(unit,&route_key,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (success != SOC_SAND_SUCCESS) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Internal error %s\n"),soc_sand_SAND_SUCCESS_FAILURE_to_string(success)));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("internal error")));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_route_remove(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv;
    bcm_error_t
        rv;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO
        route_info;
    SOC_SAND_SUCCESS_FAILURE
        success;
    bcm_ipmc_addr_t
        data_find;

    BCMDNX_INIT_FUNC_DEFS;

#if defined(INCLUDE_KBP)
    if(ARAD_KBP_ENABLE_IPV6_MC && ARAD_KBP_IPV6MC_INVERT_GROUP_SOURCE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Special case of IPv6 MC look-up in KBP is in use. bcm_ipmc_remove is disabled for IPv6 entries")));
    }
#endif

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipv6_mc_forwarding_disable", 0))
    {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("ipmc APIs is unavailable when property ipv6_mc_forwarding_disable=1")));    
    }

    unit = (unit);

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&route_info);
    bcm_ipmc_addr_t_init(&data_find);

    sal_memcpy(&data_find,data,sizeof(bcm_ipmc_addr_t));

    /* Check if entry is already existed */
    rv = bcm_petra_ipmc_find(unit,&data_find);
    
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }
        
    if (rv == BCM_E_NOT_FOUND) {
       LOG_INFO(BSL_LS_BCM_IPMC, (BSL_META_U(unit, "IPV6 entry key given does not exist\n")));
       BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND); 
    }

    /* Route key */ 
    rv = _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_route_key(unit, data, &route_key);
    BCMDNX_IF_ERR_EXIT(rv);
 
    soc_sand_rv = soc_ppd_frwrd_ipv6_mc_route_remove(unit,&route_key,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);


    if (success != SOC_SAND_SUCCESS) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Internal error %s\n"),soc_sand_SAND_SUCCESS_FAILURE_to_string(success)));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("internal error")));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_compress_sip_remove(
    int unit,
    bcm_ipmc_addr_t *data
  )
{
    uint32
        soc_sand_rv;
    bcm_error_t
        rv;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY
        route_key;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO
        route_info;
    SOC_SAND_SUCCESS_FAILURE
        success = SOC_SAND_SUCCESS;
    bcm_ipmc_addr_t
        data_find;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY_clear(&route_key);
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO_clear(&route_info);
    bcm_ipmc_addr_t_init(&data_find);

    /* Check if entry is already existed */
    sal_memcpy(&data_find, data, sizeof(bcm_ipmc_addr_t));
    rv = bcm_petra_ipmc_config_find(unit, &data_find);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "find failed\n")));
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (rv == BCM_E_NOT_FOUND) {
       LOG_INFO(BSL_LS_BCM_IPMC, (BSL_META_U(unit, "IPV6 entry key given does not exist\n")));
       BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    /* Route key */
    rv = _bcm_ipmc_addr_t_to_ppd_frwrd_ipv6_mc_compress_sip_key(unit, data, &route_key);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_frwrd_ipv6_mc_compress_sip_remove(unit,&route_key,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (success != SOC_SAND_SUCCESS) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit,
                              "Internal error %s\n"), soc_sand_SAND_SUCCESS_FAILURE_to_string(success)));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("internal error")));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_mc_routing_table_clear(
    int unit
  )
{
    uint32
        soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    soc_sand_rv = soc_ppd_frwrd_ipv4_mc_routing_table_clear(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_routing_table_clear(
    int unit
  )
{
    uint32
        soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    soc_sand_rv = soc_ppd_frwrd_ipv6_mc_routing_table_clear(unit);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_mc_route_traverse(
    int unit,
    uint32 flags,
    bcm_ipmc_traverse_cb cb,
    void *user_data
  )
{
    SOC_DPP_DBAL_SW_TABLE_IDS table_ids[] = {
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_FWD_KBP,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS
    };
    uint32 tables_num = sizeof(table_ids)/sizeof(SOC_DPP_DBAL_SW_TABLE_IDS);
    uint32
      ret;
    int i, qual_idx, is_table_initiated;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 kaps_payload = 0;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO kbp_payload;
    void *payload = NULL;
    uint8 found;
    bcm_ipmc_addr_t entry;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type;
    uint32 soc_flags = 0;
    uint8 hit_bit = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & BCM_IPMC_HIT_CLEAR) {
        if (!SOC_IS_JERICHO_PLUS(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("BCM_IPMC_HIT_CLEAR only available in Jericho plus and above.")));
        }

        soc_flags = SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    for (i=0; i<tables_num; i++) {
        ret = soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(unit, table_ids[i], &is_table_initiated);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated failed")));
        }

        if (is_table_initiated) {
            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_init(unit, table_ids[i], &physical_db_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_init failed")));
            }

            /* Per Physical DB payload */
            if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                payload = &kaps_payload;
            } else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
                payload = &kbp_payload;
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
            }

            while (found==TRUE) {
                bcm_ipmc_addr_t_init(&entry);

                /* From qual_Vals to entry */
                for (qual_idx = 0; qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_idx++) {
                    qual_type = qual_vals[qual_idx].type;
                    if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_DIP) {
                        entry.mc_ip_addr = qual_vals[qual_idx].val.arr[0];
                        entry.mc_ip_mask = qual_vals[qual_idx].is_valid.arr[0];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV4_SIP) {
                        entry.s_ip_addr = qual_vals[qual_idx].val.arr[0];
                        entry.s_ip_mask = qual_vals[qual_idx].is_valid.arr[0];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                        entry.vrf = qual_vals[qual_idx].val.arr[0];
                    }

                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_IN_RIF) {
                        entry.vid = qual_vals[qual_idx].val.arr[0];
                    }
                }

                /* Per Physical DB payload */
                if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                    if (BCM_L3_ITF_TYPE_IS_FEC(kaps_payload)) {
                        entry.l3a_intf = kaps_payload;
                    } else {
                        entry.group = kaps_payload;
                    }
                } else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
					_bcm_l3_fec_to_intf(unit, kbp_payload.fec_id, &(entry.l3a_intf));
                    entry.group = kbp_payload.frwrd_decision.dest_id;
                }

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KAPS) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV4MC_KBP_DEFAULT_ROUTE_KAPS)) {
                    entry.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
                    /* This table only contains the 28 lsbits */
                    entry.mc_ip_addr = entry.mc_ip_addr | 0xE0000000;
                    entry.mc_ip_mask = entry.mc_ip_mask | 0xF0000000;
                }

                if (hit_bit & 1) {
                    entry.flags |= BCM_IPMC_HIT;
                }

                (*cb)(unit, &entry, user_data);

                ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
                }
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(unit, table_ids[i]);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_deinit failed")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv4_mc_route_get_block(
    int unit, 
    uint32 flags, 
    bcm_ipmc_traverse_cb cb, 
    void *user_data
  )
{
    uint32
        soc_sand_rv,
        nof_entries = 0;
    SOC_PPC_IP_ROUTING_TABLE_RANGE
        block_range_key;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY
        *route_keys = NULL;
    SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO
        routes_info[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
        routes_location[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    bcm_ipmc_addr_t 
        entry;
    int idx;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range_key);

    /* Block range key */
    block_range_key.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED; /* type is not used, set only to pass verify */
    block_range_key.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range_key.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN(unit);
    
    BCMDNX_ALLOC(route_keys, sizeof(SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv4_mc_route_traverse.route_keys");
    if (route_keys == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
 
    while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&block_range_key.start.payload))
    {
      block_range_key.start.payload.arr[1] = -1; /* VRF indication. -1 for all VRFs */
      nof_entries = 0;
      /* Call function */
      soc_sand_rv = soc_ppd_frwrd_ipv4_mc_route_get_block(unit,&block_range_key,route_keys,routes_info,routes_status,routes_location,&nof_entries);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      
      for(idx = 0; idx < nof_entries; idx++)
      {
        bcm_ipmc_addr_t_init(&entry);
        /* Valid entries,            */
        /* Fill MC entry infromation */
        entry.flags         = BCM_IPMC_SOURCE_PORT_NOCHECK;
        entry.mc_ip_addr    = route_keys[idx].group;
        entry.mc_ip_mask    = bcm_ip_mask_create(route_keys[idx].group_prefix_len);
        entry.s_ip_addr     = route_keys[idx].source.ip_address;
        entry.s_ip_mask     = bcm_ip_mask_create(route_keys[idx].source.prefix_len);
        if (SOC_IS_JERICHO(unit)) {
            entry.vrf           = route_keys[idx].vrf_ndx;
        } else {
            entry.vrf           = BCM_L3_VRF_DEFAULT;
        }
        if (route_keys[idx].inrif_valid) {
            entry.vid = route_keys[idx].inrif;
        }
        else{
            entry.vid = 0;
        }
        if (routes_info[idx].dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST) {
            entry.group = routes_info[idx].dest_id.dest_val;
        }
        else if (routes_info[idx].dest_id.dest_type == SOC_SAND_PP_DEST_FEC) {
            BCM_L3_ITF_SET(entry.l3a_intf, BCM_L3_ITF_TYPE_FEC, routes_info[idx].dest_id.dest_val);
        } else {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "entry get block ipv4 resolve dest type differ from multicast\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("in IPV4 entry resolved dest-type differ from multicast")));
        }
        /* Invoke user callback. */
        (*cb)(unit, &entry, user_data);
        
       }
    }

    BCM_EXIT;
exit:
    BCM_FREE(route_keys);
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_route_traverse(
    int unit,
    uint32 flags,
    bcm_ipmc_traverse_cb cb,
    void *user_data
  )
{
    SOC_DPP_DBAL_SW_TABLE_IDS table_ids[] = {
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_FWD_KBP,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS,
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_KAPS
    };
    uint32 tables_num = sizeof(table_ids)/sizeof(SOC_DPP_DBAL_SW_TABLE_IDS);
    uint32
      ret;
    int i, qual_idx, is_table_initiated;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 kaps_payload = 0;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO kbp_payload;
    void *payload = NULL;
    uint8 found;
    bcm_ipmc_addr_t entry;
    SOC_SAND_PP_IPV6_ADDRESS ipv6_addr, ipv6_mask;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type;
    uint32 soc_flags = 0;
    uint8 hit_bit = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & BCM_IPMC_HIT_CLEAR) {
        if (!SOC_IS_JERICHO_PLUS(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("BCM_IPMC_HIT_CLEAR only available in Jericho plus and above.")));
        }

        soc_flags = SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    for (i=0; i<tables_num; i++) {
        ret = soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(unit, table_ids[i], &is_table_initiated);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated failed")));
        }

        if (is_table_initiated) {
            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_init(unit, table_ids[i], &physical_db_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_init failed")));
            }

            /* Per Physical DB payload */
            if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                payload = &kaps_payload;
            } else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
                payload = &kbp_payload;
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
            }

            while (found==TRUE) {
                bcm_ipmc_addr_t_init(&entry);

                sal_memset(&ipv6_addr,0x0,sizeof(SOC_SAND_PP_IPV6_ADDRESS));
                sal_memset(&ipv6_mask,0x0,sizeof(SOC_SAND_PP_IPV6_ADDRESS));
                /* From qual_Vals to entry */
                for (qual_idx = 0; qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_idx++) {
                    qual_type = qual_vals[qual_idx].type;
                    if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_LOW) {
                        ipv6_addr.address[0] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[1] = qual_vals[qual_idx].val.arr[1];
                        ipv6_mask.address[0] = qual_vals[qual_idx].is_valid.arr[0];
                        ipv6_mask.address[1] = qual_vals[qual_idx].is_valid.arr[1];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_DIP_HIGH) {
                        ipv6_addr.address[2] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[3] = qual_vals[qual_idx].val.arr[1];
                        ipv6_mask.address[2] = qual_vals[qual_idx].is_valid.arr[0];
                        ipv6_mask.address[3] = qual_vals[qual_idx].is_valid.arr[1];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_VRF) {
                        entry.vrf = qual_vals[qual_idx].val.arr[0];
                    }

                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_IN_RIF) {
                        entry.vid = qual_vals[qual_idx].val.arr[0];
                    }
                }
                ret = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &ipv6_addr, &(entry.mc_ip6_addr));
                BCMDNX_IF_ERR_EXIT(ret);
                ret = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &ipv6_mask, &(entry.mc_ip6_mask));
                BCMDNX_IF_ERR_EXIT(ret);

                /* Per Physical DB payload */
                if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                    if (BCM_L3_ITF_TYPE_IS_FEC(kaps_payload)) {
                        entry.l3a_intf = kaps_payload;
                    } else {
                        entry.group = kaps_payload;
                    }
                } else if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KBP) {
					_bcm_l3_fec_to_intf(unit, kbp_payload.fec_id, &(entry.l3a_intf));
                    entry.group = kbp_payload.frwrd_decision.dest_id;
                }

                if ((table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_FWD_KBP) ||
                    (table_ids[i] == SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_KBP_DEFAULT_ROUTE_KAPS)) {
                    entry.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK | BCM_IPMC_IP6;
                }

                if (hit_bit & 1) {
                    entry.flags |= BCM_IPMC_HIT;
                }

                (*cb)(unit, &entry, user_data);

                ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
                }
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(unit, table_ids[i]);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_deinit failed")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_compress_sip_traverse(
    int unit,
    uint32 flags,
    bcm_ipmc_traverse_cb cb,
    void *user_data
  )
{
    SOC_DPP_DBAL_SW_TABLE_IDS table_ids[] = {
        SOC_DPP_DBAL_SW_TABLE_ID_IPV6MC_SSM_COMPRESS_SIP_KAPS
    };
    uint32 tables_num = sizeof(table_ids) / sizeof(SOC_DPP_DBAL_SW_TABLE_IDS);
    uint32 ret;
    int i, qual_idx, is_table_initiated;
    SOC_PPC_FP_QUAL_TYPE qual_type;
    SOC_PPC_FP_QUAL_VAL qual_vals[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    uint32 kaps_payload = 0;
    void *payload = NULL;
    uint8 found = FALSE;
    bcm_ipmc_addr_t entry;
    SOC_SAND_PP_IPV6_ADDRESS ipv6_addr, ipv6_mask;
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES physical_db_type = SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS;
    uint32 soc_flags = 0;
    uint8 hit_bit = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & BCM_IPMC_HIT_CLEAR) {
        if (!SOC_IS_JERICHO_PLUS(unit)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("BCM_IPMC_HIT_CLEAR only available in Jericho plus and above.")));
        }

        soc_flags = SOC_PPC_FRWRD_IP_CLEAR_ON_GET;
    }

    for (i = 0; i < tables_num; i++) {
        ret = soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated(unit, table_ids[i], &is_table_initiated);
        if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
        {
          LOG_ERROR(BSL_LS_BCM_L3,
                    (BSL_META_U(unit,
                                "soc_sand error %x\n"), ret));
          BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_table_is_initiated failed")));
        }

        if (is_table_initiated) {
            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_init(unit, table_ids[i], &physical_db_type);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_init failed")));
            }

            /* Per Physical DB payload */
            if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                payload = &kaps_payload;
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
            }

            while (found == TRUE) {
                bcm_ipmc_addr_t_init(&entry);

                sal_memset(&ipv6_addr, 0x0, sizeof(SOC_SAND_PP_IPV6_ADDRESS));
                sal_memset(&ipv6_mask, 0x0, sizeof(SOC_SAND_PP_IPV6_ADDRESS));
                /* From qual_Vals to entry */
                for (qual_idx = 0; qual_idx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_idx++) {
                    qual_type = qual_vals[qual_idx].type;
                    if ((qual_type == SOC_PPC_NOF_FP_QUAL_TYPES) || (qual_type == BCM_FIELD_ENTRY_INVALID)) {
                        break;
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_LOW) {
                        ipv6_addr.address[0] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[1] = qual_vals[qual_idx].val.arr[1];
                        ipv6_mask.address[0] = qual_vals[qual_idx].is_valid.arr[0];
                        ipv6_mask.address[1] = qual_vals[qual_idx].is_valid.arr[1];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_HDR_FWD_IPV6_SIP_HIGH) {
                        ipv6_addr.address[2] = qual_vals[qual_idx].val.arr[0];
                        ipv6_addr.address[3] = qual_vals[qual_idx].val.arr[1];
                        ipv6_mask.address[2] = qual_vals[qual_idx].is_valid.arr[0];
                        ipv6_mask.address[3] = qual_vals[qual_idx].is_valid.arr[1];
                    }
                    if (qual_type == SOC_PPC_FP_QUAL_IRPP_IN_RIF) {
                        entry.vid = qual_vals[qual_idx].val.arr[0];
                    }
                }
                ret = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &ipv6_addr, &(entry.s_ip6_addr));
                BCMDNX_IF_ERR_EXIT(ret);
                ret = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit, &ipv6_mask, &(entry.s_ip6_mask));
                BCMDNX_IF_ERR_EXIT(ret);

                /* Per Physical DB payload */
                if (physical_db_type == SOC_DPP_DBAL_PHYSICAL_DB_TYPE_KAPS) {
                    entry.s_ip_addr = kaps_payload;
                }

                entry.flags |= BCM_IPMC_IP6;

                if (hit_bit & 1) {
                    entry.flags |= BCM_IPMC_HIT;
                }

                (*cb)(unit, &entry, user_data);

                ret = soc_ppd_frwrd_ipv4_uc_route_iterator_get_next(unit, table_ids[i], soc_flags, qual_vals, payload, NULL/*priority*/, &hit_bit, &found);
                if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
                {
                  LOG_ERROR(BSL_LS_BCM_L3,
                            (BSL_META_U(unit,
                                        "soc_sand error %x\n"), ret));
                  BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_get_next failed")));
                }
            }

            ret = soc_ppd_frwrd_ipv4_uc_route_iterator_deinit(unit, table_ids[i]);
            if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
            {
              LOG_ERROR(BSL_LS_BCM_L3,
                        (BSL_META_U(unit,
                                    "soc_sand error %x\n"), ret));
              BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("soc_ppd_frwrd_ipv4_uc_route_iterator_deinit failed")));
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
  _bcm_ppd_frwrd_ipv6_mc_route_get_block(
    int unit, 
    uint32 flags, 
    bcm_ipmc_traverse_cb cb, 
    void *user_data
  )
{
    bcm_error_t
        rv;
    uint32
        soc_sand_rv,
        nof_entries = 0;
    SOC_PPC_IP_ROUTING_TABLE_RANGE
        block_range_key;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY
        *route_keys = NULL;
    SOC_PPC_FRWRD_IPV6_MC_ROUTE_INFO
        routes_info[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
        routes_status[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
        routes_location[DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES];
    bcm_ipmc_addr_t 
        entry;
    int idx;

    BCMDNX_INIT_FUNC_DEFS;

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ipv6_mc_forwarding_disable", 0))
    {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("ipV6mc APIs is unavailable when property ipv6_mc_forwarding_disable=1")));    
    }

    SOC_PPC_IP_ROUTING_TABLE_RANGE_clear(&block_range_key);

    /* Block range key */
    block_range_key.start.type = SOC_PPC_IP_ROUTING_TABLE_ITER_TYPE_IP_PREFIX_ORDERED;
    block_range_key.entries_to_act = DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES;
    block_range_key.entries_to_scan = DPP_FRWRD_IP_ENTRIES_TO_SCAN(unit);
    
    BCMDNX_ALLOC(route_keys, sizeof(SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES, "_bcm_ppd_frwrd_ipv6_mc_route_traverse.route_keys");
    if (route_keys == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("failed to allocate memory")));
    }
 
    while(!SOC_PPC_IP_ROUTING_TABLE_ITER_IS_END(&block_range_key.start.payload))
    {
      block_range_key.start.payload.arr[1] = -1; /* VRF indication. -1 for all VRFs */
      nof_entries = 0;
      sal_memset(route_keys, 0x0, sizeof(SOC_PPC_FRWRD_IPV6_MC_ROUTE_KEY)*DPP_FRWRD_IP_GET_BLOCK_NOF_ENTRIES);
      /* Call function */
      soc_sand_rv = soc_ppd_frwrd_ipv6_mc_route_get_block(unit,&block_range_key,route_keys,routes_info,routes_status,routes_location,&nof_entries);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      
      for(idx = 0; idx < nof_entries; idx++)
      {
        bcm_ipmc_addr_t_init(&entry);
        /* Valid entries,            */
        /* Fill MC entry infromation */
        entry.flags         = BCM_IPMC_SOURCE_PORT_NOCHECK | BCM_IPMC_IP6;
        rv = _bcm_l3_sand_ipv6_addr_to_bcm_ipv6_addr(unit,&(route_keys[idx].group.ipv6_address),&(entry.mc_ip6_addr));
        BCMDNX_IF_ERR_EXIT(rv);

        if (route_keys[idx].group.prefix_len != 0) {
            rv = _shr_ip6_mask_create(entry.mc_ip6_mask, route_keys[idx].group.prefix_len);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
        if (SOC_IS_JERICHO(unit)) {
            entry.vrf = route_keys[idx].vrf_ndx;
        } else {
            entry.vrf           = BCM_L3_VRF_DEFAULT;
        }
        entry.vid = route_keys[idx].inrif.val & route_keys[idx].inrif.mask;
        
        if (routes_info[idx].dest_id.dest_type == SOC_SAND_PP_DEST_MULTICAST) {
            entry.group = routes_info[idx].dest_id.dest_val;
        }
        else if (routes_info[idx].dest_id.dest_type == SOC_SAND_PP_DEST_FEC) {
            BCM_L3_ITF_SET(entry.l3a_intf, BCM_L3_ITF_TYPE_FEC, routes_info[idx].dest_id.dest_val);
        } else {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                      (BSL_META_U(unit,
                                  "entry get block ipv6 resolve dest type differ from multicast\n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("in IPV6 entry resolved dest-type differ from multicast")));
        }
        /* Invoke user callback. */
        (*cb)(unit, &entry, user_data);
        
       }
    }

    BCM_EXIT;
exit:
    BCM_FREE(route_keys);
    BCMDNX_FUNC_RETURN;
}
       
/***************************************************************/
/***************************************************************/


/*
 * Function:
 *      bcm_petra_ipmc_init
 * Purpose:
 *      Initialize the BCM IPMC subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_init(
    int unit)
{
    uint8 is_allocated;
    bcm_error_t  rv = BCM_E_NONE;   
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (!SOC_DPP_CONFIG(unit)->pp.ipmc_enable) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("IPMC is disabled. while ipmc init being called"))); 
    }

    if (!SOC_WARM_BOOT(unit)) {
        rv = IPMC_ACCESS.is_allocated(unit, &is_allocated);
        BCMDNX_IF_ERR_EXIT(rv);

        if(!is_allocated) {
            rv = IPMC_ACCESS.alloc(unit);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        rv = bcm_petra_ipmc_enable(unit, 1); /* Enable IPMC Support */
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_petra_ipmc_add
 * Purpose:
 *      Add new IPMC group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_add(
    int unit, 
    bcm_ipmc_addr_t *data)
{
    int  status;
    uint8 bridged=0;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;

    BCMDNX_NULL_CHECK(data);
    BCM_DPP_UNIT_CHECK(unit);

    status = _bcm_petra_ipmc_addr_validate(unit, data);
    BCMDNX_IF_ERR_EXIT(status);
    
    if (!(data->flags & BCM_IPMC_SOURCE_PORT_NOCHECK)) {
        /* Check valid Source port */
        status = _bcm_petra_source_port_check(unit, data);
        BCMDNX_IF_ERR_EXIT(status);
    }
  
    if (data->flags & BCM_IPMC_IP6) {
        /* IPV6 */
        status = _bcm_ppd_frwrd_ipv6_mc_route_add(unit, data);
        BCMDNX_IF_ERR_EXIT(status);
    } else {
        /* IPV4 */
        status = _bcm_ppd_frwrd_ipv4_mc_is_bridged(unit,data,&bridged);
        BCMDNX_IF_ERR_EXIT(status);
        /* check if this ipmc entry is to be bridged */
        if (bridged) {
            status = _bcm_ppd_frwrd_ipv4_mc_bridge_add(unit, data);
            BCMDNX_IF_ERR_EXIT(status);
        } else {
            if (data->vid >= SOC_DPP_CONFIG(unit)->l3.nof_rifs) {
                LOG_ERROR(BSL_LS_BCM_IPMC,
                          (BSL_META_U(unit,
                                      "Invalid vid\n")));
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("invalid vid")));
            }          
            status = _bcm_ppd_frwrd_ipv4_mc_route_add(unit, data);
            BCMDNX_IF_ERR_EXIT(status);
        }
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_detach
 * Purpose:
 *      Detach the BCM IPMC subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_detach(
    int unit)
{
    bcm_error_t rv;
    int init;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;

    rv = IPMC_ACCESS.is_allocated(unit, &is_allocated);
    BCMDNX_IF_ERR_EXIT(rv);

    if(!is_allocated) {
        BCM_EXIT;
    }
    rv = IPMC_ACCESS.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);
    
    if (init) {
        /* Remove all IPMC entries */
        rv = bcm_petra_ipmc_remove_all(unit);
        BCMDNX_IF_ERR_CONT(rv);

        /* Disable IPMC system */
        rv = bcm_petra_ipmc_enable(unit, 0);
        BCMDNX_IF_ERR_CONT(rv);
    }

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_enable
 * Purpose:
 *      Enable/disable IPMC support.
 * Parameters:
 *      unit - (IN) Unit number.
 *      enable - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_enable(
    int unit, 
    int enable)
{

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_IF_ERR_EXIT(IPMC_ACCESS.init.set(unit, enable));

    
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_find
 * Purpose:
 *      Find info of an IPMC group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      data - (IN/OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_find(
    int unit, 
    bcm_ipmc_addr_t *data)
{
    int status;
    uint8 bridged;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(data);

    status = _bcm_petra_ipmc_addr_validate(unit, data);
    BCMDNX_IF_ERR_EXIT(status);


    if (data->flags & BCM_IPMC_IP6) {
        /* IPV6 */
        status = _bcm_ppd_frwrd_ipv6_mc_route_find(unit, data);
        if(status != BCM_E_NOT_FOUND && status != BCM_E_NONE){
            BCMDNX_IF_ERR_EXIT(status);/*return value unexpected, print the error and exit*/
        }
        BCM_RETURN_VAL_EXIT(status);/*return value expected, return the error for further use without printing*/
    } else {
        /* IPV4 */

        status = _bcm_ppd_frwrd_ipv4_mc_is_bridged(unit,data,&bridged);
        BCMDNX_IF_ERR_EXIT(status);

        /* check if this ipmc entry is to be bridged */
       if (bridged) {
            status =_bcm_ppd_frwrd_ipv4_mc_bridged_find(unit,data);
            if(status != BCM_E_NOT_FOUND && status != BCM_E_NONE){
                BCMDNX_IF_ERR_EXIT(status);/*return value unexpected, print the error and exit*/
            }
            BCM_RETURN_VAL_EXIT(status);/*return value expected, return the error for further use without printing*/
        }
       else{
            status = _bcm_ppd_frwrd_ipv4_mc_route_find(unit, data);
            if(status != BCM_E_NOT_FOUND && status != BCM_E_NONE){
                BCMDNX_IF_ERR_EXIT(status);/*return value unexpected, print the error and exit*/
            }
            BCM_RETURN_VAL_EXIT(status);/*return value expected, return the error for further use without printing*/
        }
    }  

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_remove
 * Purpose:
 *      Remove IPMC group.
 * Parameters:
 *      unit - (IN) Unit number.
 *      data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_remove(
    int unit, 
    bcm_ipmc_addr_t *data)
{
    int status;
    uint8 bridged;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;

    BCMDNX_NULL_CHECK(data);
    BCM_DPP_UNIT_CHECK(unit);

    status = _bcm_petra_ipmc_addr_validate(unit, data);
    BCMDNX_IF_ERR_EXIT(status);

    if (data->flags & BCM_IPMC_IP6) {
        /* IPV6 */
        status = _bcm_ppd_frwrd_ipv6_mc_route_remove(unit, data);
        BCMDNX_IF_ERR_EXIT(status); 
    } else {
        /* IPV4 */
        /* check if this ipmc entry is to be bridged */
        status = _bcm_ppd_frwrd_ipv4_mc_is_bridged(unit,data,&bridged);
        BCMDNX_IF_ERR_EXIT(status);

        if (bridged) {
             status = _bcm_ppd_frwrd_ipv4_mc_bridge_remove(unit, data);
             BCMDNX_IF_ERR_EXIT(status);
         }
        else{
             status = _bcm_ppd_frwrd_ipv4_mc_route_remove(unit, data);
             BCMDNX_IF_ERR_EXIT(status);
         }
    }  
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_remove_all
 * Purpose:
 *      Remove all IPMC groups.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_remove_all(
    int unit)
{
    bcm_error_t
        rv;
    BCMDNX_INIT_FUNC_DEFS;
           
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;
    
    if (!(SOC_DPP_CONFIG(unit)->pp.ipmc_enable)) {
        return BCM_E_UNAVAIL;
    }

    /* Remove all IPV4 entries */
    rv = _bcm_ppd_frwrd_ipv4_mc_routing_table_clear(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Remove all IPV6 entries */
    rv = _bcm_ppd_frwrd_ipv6_mc_routing_table_clear(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /*Remove all L2 entries*/
    rv = _bcm_ppd_frwrd_ipv4_mc_bridge_clear(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_traverse
 * Purpose:
 *      Traverse through the ipmc table and run callback at each valid
 *      ipmc entry.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) <UNDEF>
 *      cb - (IN) <UNDEF>
 *      user_data - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_ipmc_traverse(
    int unit, 
    uint32 flags, 
    bcm_ipmc_traverse_cb cb, 
    void *user_data)
{
    bcm_error_t
        rv;
    BCMDNX_INIT_FUNC_DEFS;

    DPP_IPMC_UNIT_VALID_CHECK;

    if (flags & ~IPMC_SUPPORTED_FLAGS(unit)) {
       LOG_ERROR(BSL_LS_BCM_IPMC,
                 (BSL_META_U(unit,
                             "flags %x is not supported\n"), flags));
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("one or more flags is not supported")));
    }

    /* Traverse only the MC routing table */
    if (flags & BCM_IPMC_IP6) {
        /* IPV6 */
#if defined(INCLUDE_KBP)
        if (SOC_IS_JERICHO(unit) && (!(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mc_use_tcam", 0)) || ARAD_KBP_ENABLE_IPV6_MC)) {
            rv = _bcm_ppd_frwrd_ipv6_mc_route_traverse(unit, flags, cb, user_data);
        } else
#endif /*defined(INCLUDE_KBP)*/
        {
            rv = _bcm_ppd_frwrd_ipv6_mc_route_get_block(unit, flags, cb, user_data);
        }
    } else {
        /* IPV4 */
#if defined(INCLUDE_KBP)
        if (SOC_IS_JERICHO(unit) && !((soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "l3_mc_use_tcam", 0)) || SOC_DPP_CONFIG(unit)->pp.ipmc_l2_ssm_mode)) {
            rv = _bcm_ppd_frwrd_ipv4_mc_route_traverse(unit, flags, cb, user_data);
        } else
#endif /*defined(INCLUDE_KBP)*/
        {
            rv = _bcm_ppd_frwrd_ipv4_mc_route_get_block(unit, flags, cb, user_data);
        }

        if (flags & BCM_IPMC_L2) {
            /*bridge*/
            rv = _bcm_ppd_frwrd_ipv4_mc_bridge_get_block(unit, flags, cb, user_data);
        }
    }
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    _DCMN_BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_ipmc_rp_support_check(
        uint32 unit)
{
    BCMDNX_INIT_FUNC_DEFS;

    /* device support check */
    if (_BCM_DPP_IPMC_NOF_RPS(unit) == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Feature disabled, check soc-property ipmc_pim_mode")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* RP implementation for BIDIR-PIM */

/*
 * Function:
 *      bcm_petra_ipmc_rp_create
 * Purpose:
 *      Create a rendezvous point.
 * Parameters:
 *      unit - unit #
 *      flags - BCM_IPMC_RP_xxx flags.
 *      rp_id - (IN/OUT) Rendezvous point ID.
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_petra_ipmc_rp_create(
    int unit, 
    uint32 flags, 
    int *rp_id)
{
    int rv = BCM_E_NONE;
    uint32 shr_flags = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;

    BCMDNX_NULL_CHECK(rp_id);

    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* with ID */
    if (flags & BCM_IPMC_RP_WITH_ID) {
        if ((*rp_id < 0) || (*rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RP-id out of range")));
        }
        shr_flags |= BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
    } 

    /* allocate */
    rv = bcm_dpp_am_rp_alloc(unit,shr_flags,rp_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_ipmc_rp_destroy
 * Purpose:
 *      Destroy a rendezvous point.
 * Parameters:
 *      unit - unit #
 *      rp_id - Rendezvous point ID.
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_petra_ipmc_rp_destroy(
    int unit, 
    int rp_id)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;

    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((rp_id < 0) || (rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit))) {
        return BCM_E_PARAM;
    }

    rv = bcm_petra_ipmc_rp_delete_all(unit,rp_id);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_dpp_am_rp_dealloc(unit,rp_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
bcm_petra_ipmc_range_key_to_ppd(
    int unit, 
    int                         *range_id, 
    bcm_ipmc_range_t            *range,
    SOC_PPC_RIF_IP_TERM_KEY     *term_key
 ) 
{
    uint32 rv;
    BCMDNX_INIT_FUNC_DEFS;


    /* Input check */
    BCMDNX_NULL_CHECK(range_id);
    BCMDNX_NULL_CHECK(range);

    SOC_PPC_RIF_IP_TERM_KEY_clear(term_key);
    /* verify inputs */
    if((range->mc_ip_addr & 0xf0000000) != 0xe0000000){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IP address is not Multicast")));
    }

    if (!(range->flags & BCM_IPMC_RANGE_PIM_BIDIR)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("only PIM-BIDIR eligible addresses can be specified.")));
    }

    if ((range->flags & (BCM_IPMC_RANGE_REPLACE|BCM_IPMC_RANGE_REPLACE|BCM_IPMC_RANGE_WITH_ID))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported flag")));
    }

    if (range->priority != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("priority has to be zero, not used")));
    }
    
    if (range->rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RP-id out of range")));
    }

    if (range->vrf >= SOC_DPP_CONFIG(unit)->l3.nof_bidir_vrfs){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VRF ID out of range for BIDIR support")));
    }

    /* exact match support only thus vrf and sip all considered */
    if (range->vrf_mask != (int)-1 && range->mc_ip_addr_mask != (int)-1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("SIP,VRF mask not supported ")));
    }

    rv = bcm_dpp_am_rp_is_alloced(unit,range->rp_id);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(rv);
    }
    rv = BCM_E_NONE;

    term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY|SOC_PPC_RIF_IP_TERM_IPMC_BIDIR;
    term_key->vrf = range->vrf;
    term_key->dip = range->mc_ip_addr;

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int
bcm_petra_ipmc_entry_rp_to_ppd(
    int unit, 
    bcm_ipmc_addr_t             *entry,
    SOC_PPC_RIF_IP_TERM_KEY     *term_key
 ) 
{
    BCMDNX_INIT_FUNC_DEFS;

    /* Input check */
    BCMDNX_NULL_CHECK(term_key);
    BCMDNX_NULL_CHECK(entry);

    SOC_PPC_RIF_IP_TERM_KEY_clear(term_key);
    /* verify inputs */
    if((entry->mc_ip_addr & 0xf0000000) != 0xe0000000){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("IP address is not Multicast")));
    }

    if (entry->vrf >= SOC_DPP_CONFIG(unit)->l3.nof_bidir_vrfs){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VRF ID out of range for BIDIR support")));
    }

    if (entry->vid != 0 || entry->s_ip_addr != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VID and SIP has to be 0 for RP entry")));
    }


    term_key->flags = SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY|SOC_PPC_RIF_IP_TERM_IPMC_BIDIR;
    term_key->vrf = entry->vrf;
    term_key->dip = entry->mc_ip_addr;

exit:
    BCMDNX_FUNC_RETURN;
}





STATIC int 
bcm_petra_ipmc_entry_rp_add(
    int unit, 
    bcm_ipmc_addr_t *entry)
{
    uint32 rv= BCM_E_NONE;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    SOC_SAND_SUCCESS_FAILURE success;
    SOC_PPC_FRWRD_IPV4_HOST_KEY host_key;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO routing_info;
    int lif_indx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;
    
    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_dev_id = (unit);

    /* RP DB: <VRF,G> --> RP*/
    SOC_PPC_RIF_IP_TERM_INFO_clear(&term_info);

    if (entry->rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RP-id out of range")));
    }

    rv = bcm_dpp_am_rp_is_alloced(unit,entry->rp_id);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(rv);
    }
    rv = bcm_petra_ipmc_entry_rp_to_ppd(unit,entry,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    lif_indx = entry->rp_id;
    term_info.ext_lif_id = lif_indx;
    soc_sand_rv = 
    soc_ppd_rif_ip_tunnel_term_add(soc_sand_dev_id, 0, &term_key, lif_indx,&term_info,  NULL,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);


    /* FRWRD DB: <VRF,G> --> MC-group*/
    if (_BCM_MULTICAST_IS_SET(entry->group)) {
        SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&host_key);
        SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO_clear(&routing_info);

        SOC_PPD_FRWRD_DECISION_MC_GROUP_SET(soc_sand_dev_id,&routing_info.frwrd_decision,_BCM_MULTICAST_ID_GET(entry->group), soc_sand_rv);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        host_key.vrf_ndx = entry->vrf;
        host_key.ip_address = entry->mc_ip_addr;
        soc_sand_rv = soc_ppd_frwrd_ipv4_host_add(
                soc_sand_dev_id,
                &host_key,
                &routing_info,
                &success
              );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        SOC_SAND_IF_FAIL_RETURN(success);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int 
bcm_petra_ipmc_entry_rp_remove(
    int unit, 
    bcm_ipmc_addr_t *entry,
    uint8 *is_bidir)
{
    uint32 rv= BCM_E_NONE;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_FRWRD_IPV4_HOST_KEY host_key;
    uint32 lif_indx=0xFFFFFFFF;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;
    
    *is_bidir = 0;
    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_dev_id = (unit);

    /* RP DB: <VRF,G> --> RP*/
    rv = bcm_petra_ipmc_entry_rp_to_ppd(unit,entry,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = 
    soc_ppd_rif_ip_tunnel_term_remove(soc_sand_dev_id, &term_key, &lif_indx);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (lif_indx == 0xFFFFFFFF) {
        BCM_EXIT;/* not bidir */
    }

    *is_bidir = 1;

    /* FRWRD DB: <VRF,G> --> MC-group*/
    SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&host_key);

    host_key.vrf_ndx = entry->vrf;
    host_key.ip_address = entry->mc_ip_addr;
    soc_sand_rv = soc_ppd_frwrd_ipv4_host_remove(
            soc_sand_dev_id,
            &host_key
          );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC int 
bcm_petra_ipmc_entry_rp_get(
    int unit, 
    bcm_ipmc_addr_t *entry,
    uint8 *is_bidir)
{
    uint32 rv= BCM_E_NONE;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_FRWRD_IPV4_HOST_KEY host_key;
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    SOC_PPC_RIF_INFO rif_info;
    uint8 found;
    SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO
      routing_info;
    SOC_PPC_FRWRD_IP_ROUTE_STATUS
      route_status;
    SOC_PPC_FRWRD_IP_ROUTE_LOCATION
      location;
    uint32 host_flags=0;
    uint32 lif_indx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;
    
    *is_bidir = 0;
    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_dev_id = (unit);

    /* RP DB: <VRF,G> --> RP*/
    rv = bcm_petra_ipmc_entry_rp_to_ppd(unit,entry,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    soc_sand_rv = soc_ppd_rif_ip_tunnel_term_get(soc_sand_dev_id, &term_key, &lif_indx,
                    &term_info, &rif_info, &found);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (!found) {
        BCM_EXIT;/* not bidir */
    }

    *is_bidir = 1;
    entry->rp_id = lif_indx;

    /* FRWRD DB: <VRF,G> --> MC-group*/
    SOC_PPC_FRWRD_IPV4_HOST_KEY_clear(&host_key);

    host_key.vrf_ndx = entry->vrf;
    host_key.ip_address = entry->mc_ip_addr;
    soc_sand_rv = soc_ppd_frwrd_ipv4_host_get(
            soc_sand_dev_id,
            &host_key,
            host_flags,
            &routing_info,
            &route_status,
            &location,
            &found
          );
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    if (found == SOC_SAND_FALSE) {
      /* Entry not found */
      LOG_INFO(BSL_LS_BCM_IPMC, (BSL_META_U(unit, "<VRF,G> --> MC-group not found\n")));
      BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND); 
    }

    if (routing_info.frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_MC) {
        _BCM_MULTICAST_GROUP_SET(entry->group,_BCM_MULTICAST_TYPE_L3,routing_info.frwrd_decision.dest_id);
    }
    else if (routing_info.frwrd_decision.type == SOC_PPC_FRWRD_DECISION_TYPE_FEC) {
        BCM_L3_ITF_SET(entry->l3a_intf, BCM_L3_ITF_TYPE_FEC, routing_info.frwrd_decision.dest_id);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("resolved dest-type differ from multicast")));
    }


exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_ipmc_range_add(
    int unit, 
    int *range_id, 
    bcm_ipmc_range_t *range)
{
    uint32 rv= BCM_E_NONE;
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_RIF_IP_TERM_KEY term_key;
    SOC_PPC_RIF_IP_TERM_INFO term_info;
    SOC_SAND_SUCCESS_FAILURE success;
    int lif_indx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;
    
    /* device support check */
    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = bcm_dpp_am_rp_is_alloced(unit,range->rp_id);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_dev_id = (unit);
    SOC_PPC_RIF_IP_TERM_INFO_clear(&term_info);

    rv = bcm_petra_ipmc_range_key_to_ppd(unit,range_id,range,&term_key);
    BCMDNX_IF_ERR_EXIT(rv);

    lif_indx = range->rp_id;
    soc_sand_rv = 
    soc_ppd_rif_ip_tunnel_term_add(soc_sand_dev_id, 0, &term_key, lif_indx,&term_info,  NULL,&success);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    SOC_SAND_IF_FAIL_RETURN(success);

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_ipmc_range_delete(
    int unit, 
    int range_id)
{
    return BCM_E_UNAVAIL;
}


int 
bcm_petra_ipmc_range_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}


int 
bcm_petra_ipmc_range_get(
    int unit, 
    int range_id, 
    bcm_ipmc_range_t *range)
{
    return BCM_E_UNAVAIL;
}


int 
bcm_petra_ipmc_range_size_get(
    int unit, 
    int *size)
{
    return BCM_E_UNAVAIL;
}


int 
bcm_petra_ipmc_rp_add(
    int unit, 
    int rp_id, 
    bcm_if_t intf_id)
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_FRWRD_IP_MC_RP_INFO rp_info;
    uint32 rv = BCM_E_NONE;
    

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;

    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* check params */
    if (rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RP-id out of range")));
    }
    /* check params configs */
    rv = bcm_dpp_am_rp_is_alloced(unit,rp_id);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_dev_id = (unit);
    SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_info);

    rp_info.flags = 0;
    rp_info.inrif = BCM_L3_ITF_VAL_GET(intf_id);
    rp_info.rp_id = rp_id;

    if (rp_info.inrif > SOC_DPP_CONFIG(unit)->l3.nof_rifs-1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf-ID out of range")));
    }

    soc_sand_rv = soc_ppd_frwrd_ipmc_rp_add(soc_sand_dev_id,&rp_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_ipmc_rp_delete(
    int unit, 
    int rp_id, 
    bcm_if_t intf_id)
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_FRWRD_IP_MC_RP_INFO rp_info;
    uint32 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;
    
    /* device support check */
    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* check params */
    if (rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RP-id out of range")));
    }
    if (intf_id < 0 || intf_id > SOC_DPP_CONFIG(unit)->l3.nof_rifs-1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("intf-ID out of range")));
    }
    /* check params configs */
    rv = bcm_dpp_am_rp_is_alloced(unit,rp_id);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_dev_id = (unit);
    SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_info);

    rp_info.flags = 0;
    rp_info.inrif = BCM_L3_ITF_VAL_GET(intf_id);
    rp_info.rp_id = rp_id;

    soc_sand_rv = soc_ppd_frwrd_ipmc_rp_remove(soc_sand_dev_id,&rp_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_ipmc_rp_delete_all(
    int unit, 
    int rp_id)
{

    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_PPC_FRWRD_IP_MC_RP_INFO rp_info;
    uint32 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;
    
    /* device support check */
    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* check params range*/
    if (rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RP-id out of range")));
    }

    /* check params configs */
    rv = bcm_dpp_am_rp_is_alloced(unit,rp_id);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_dev_id = (unit);
    SOC_PPC_FRWRD_IP_MC_RP_INFO_clear(&rp_info);

    /* delete all on RP ID*/
    rp_info.flags = SOC_PPC_FRWRD_IP_MC_BIDIR_IGNORE_RIF;
    rp_info.inrif = 0;
    rp_info.rp_id = rp_id;

    soc_sand_rv = soc_ppd_frwrd_ipmc_rp_remove(soc_sand_dev_id,&rp_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_ipmc_rp_get(
    int unit, 
    int rp_id, 
    int intf_max, 
    bcm_if_t *intf_array, 
    int *intf_count)
{
    int soc_sand_rv;
    unsigned int soc_sand_dev_id;
    SOC_SAND_TABLE_BLOCK_RANGE block_range;
    uint32 indx;
    uint32 rif_count=0;
    uint32 rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;

    /* device support check */
    rv = bcm_petra_ipmc_rp_support_check(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* check params range*/
    if (rp_id >= _BCM_DPP_IPMC_NOF_RPS(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RP-id out of range")));
    }

    /* check params configs */
    rv = bcm_dpp_am_rp_is_alloced(unit,rp_id);
    if (rv != BCM_E_EXISTS) {
        BCMDNX_IF_ERR_EXIT(rv);
    }

    soc_sand_dev_id = (unit);

    soc_sand_SAND_TABLE_BLOCK_RANGE_clear(&block_range);
    block_range.entries_to_act = intf_max;
    block_range.entries_to_scan = SOC_SAND_TBL_ITER_SCAN_ALL;

    soc_sand_rv = soc_ppd_frwrd_ipmc_rp_get_block(soc_sand_dev_id,rp_id,&block_range,intf_array,&rif_count);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *intf_count = (int32)rif_count;
    /* set type intf */
    for (indx = 0; indx < *intf_count; ++indx) {
        BCM_L3_ITF_SET(intf_array[indx],BCM_L3_ITF_TYPE_RIF,intf_array[indx]);
    }


exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_ipmc_rp_set(
    int unit, 
    int rp_id, 
    int intf_count, 
    bcm_if_t *intf_array)
{
    uint32 rv = BCM_E_NONE;
    uint32 indx;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    DPP_IPMC_UNIT_VALID_CHECK;

    rv = bcm_petra_ipmc_rp_delete_all(unit,rp_id);
    BCMDNX_IF_ERR_EXIT(rv);

    for (indx = 0; indx< intf_count; ++indx) {
        rv = bcm_petra_ipmc_rp_add(unit,rp_id,intf_array[indx]);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validate general ipmc structure */
int
  _bcm_petra_ipmc_config_validate(
    int unit,
    bcm_ipmc_addr_t *data,
    int is_add
  )
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit) || !SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)) {
        SOCDNX_EXIT_WITH_ERR
            (SOC_E_PARAM, (_BSL_SOCDNX_MSG(
                 "IPv6 SSM in KAPS supported from Jericho with SOC ipv6_compressed_sip_enable set.")));
    }

    if (data->flags & (~IPMC_CONFIG_SUPPORTED_FLAGS)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit, "flags %x is not supported\n"), data->flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("one or more flags is not supported")));
    }

    /* Verify that the ETH-RIF value is valid. */
    if (data->vid > (SOC_DPP_CONFIG(unit)->l3.nof_rifs - 1)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                 (BSL_META_U(unit, "Illegal ETH-RIF(vid)\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("Illegal ETH-RIF(vid")));
    }

    if (BCM_IP6_MULTICAST(data->s_ip6_addr)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                 (BSL_META_U(unit, "Source ipv6 address is mc\n")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("IPV6 source address is MC")));
    }

    if (is_add == 1) {
        /* data->s_ip_addr is IPv6 compressed SIP */
        if(data->s_ip_addr == 0) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                     (BSL_META_U(unit,
                      " Feature IPv6 SSM in KAPS must set s_ip_addr != 0 \n")));
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                (_BSL_BCM_MSG_NO_UNIT("Feature IPv6 SSM in KAPS must set s_ip_addr != 0")));
        }

        if (data->s_ip_addr > ((1 << _SHR_L3_IP6_COMPRESSED_SIP_MAX_NETLEN) -1)) {
            LOG_ERROR(BSL_LS_BCM_IPMC,
                (BSL_META_U(unit,
                     "IPv6 compressed SIP is bigger than MAX value:%d\n"),
                         ((1 << _SHR_L3_IP6_COMPRESSED_SIP_MAX_NETLEN) -1)));
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                (_BSL_BCM_MSG_NO_UNIT("IPv6 compressed SIP is bigger than MAX value:"),
                     ((1 << _SHR_L3_IP6_COMPRESSED_SIP_MAX_NETLEN) -1)));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Validate general ipmc config traverse */
int
  _bcm_petra_ipmc_config_traverse_validate(
    int unit,
    uint32 flags,
    bcm_ipmc_config_traverse_cb cb,
    void *user_data)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_IS_JERICHO(unit) || !SOC_DPP_L3_IPV6_COMPRESSED_SIP_ENABLE(unit)) {
        SOCDNX_EXIT_WITH_ERR
            (SOC_E_PARAM, (_BSL_SOCDNX_MSG(
                 "IPv6 SSM in KAPS supported from Jericho with SOC ipv6_compressed_sip_enable set.")));
    }

    if (flags & (~IPMC_CONFIG_SUPPORTED_TRAVERSE_FLAGS)) {
        LOG_ERROR(BSL_LS_BCM_IPMC,
                  (BSL_META_U(unit, "flags %x is not supported\n"), flags));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG_NO_UNIT("one or more flags is not supported")));
    }

    BCMDNX_NULL_CHECK(cb);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_config_add
 * Purpose:
 *      Add compressed IPv6 SIP.
 * Parameters:
 *      unit - (IN) Unit number.
 *      data - (IN) structure that contains the information for new entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_ipmc_config_add(
    int unit,
    bcm_ipmc_addr_t *data)
{
    int  status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;

    BCMDNX_NULL_CHECK(data);
    BCM_DPP_UNIT_CHECK(unit);

    status = _bcm_petra_ipmc_config_validate(unit, data, 1 /* is_add */);
    BCMDNX_IF_ERR_EXIT(status);

    status = _bcm_ppd_frwrd_ipv6_mc_compress_sip_add(unit, data);
    BCMDNX_IF_ERR_EXIT(status);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_config_find
 * Purpose:
 *      Find compressed IPv6 SIP.
 * Parameters:
 *      unit - (IN) Unit number.
 *      data - (IN) structure that contains the information for new entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_ipmc_config_find(
    int unit,
    bcm_ipmc_addr_t *data)
{
    int  status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;

    BCMDNX_NULL_CHECK(data);
    BCM_DPP_UNIT_CHECK(unit);

    status = _bcm_petra_ipmc_config_validate(unit, data, 0 /* is_add */);
    BCMDNX_IF_ERR_EXIT(status);

    status = _bcm_ppd_frwrd_ipv6_mc_compress_sip_find(unit, data);
    /* return value unexpected, print the error and exit */
    if((status != BCM_E_NOT_FOUND) && (status != BCM_E_NONE)){
        BCMDNX_IF_ERR_EXIT(status);
    }

    /* return value expected, return the error for further use without printing */
    BCM_RETURN_VAL_EXIT(status);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_config_remove
 * Purpose:
 *      Remove existing compressed IPv6 SIP.
 * Parameters:
 *      unit - (IN) Unit number.
 *      data - (IN) structure that contains the information for new entry.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_ipmc_config_remove(
    int unit,
    bcm_ipmc_addr_t *data)
{
    int  status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;

    BCMDNX_NULL_CHECK(data);
    BCM_DPP_UNIT_CHECK(unit);

    status = _bcm_petra_ipmc_config_validate(unit, data, 0 /* is_add */);
    BCMDNX_IF_ERR_EXIT(status);

    status = _bcm_ppd_frwrd_ipv6_mc_compress_sip_remove(unit, data);
    BCMDNX_IF_ERR_EXIT(status);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_ipmc_config_traverse
 * Purpose:
 *      Traverse all existing compressed IPv6 SIP.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) flags.
 *      cb - (IN) callback for traverse.
 *      data - (OUT) structure that contains the entry information.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_petra_ipmc_config_traverse(
    int unit,
    uint32 flags,
    bcm_ipmc_config_traverse_cb cb,
    void *user_data)
{
    int  status = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    DPP_IPMC_UNIT_VALID_CHECK;

    BCM_DPP_UNIT_CHECK(unit);

    status = _bcm_petra_ipmc_config_traverse_validate(unit, flags, cb, user_data);
    BCMDNX_IF_ERR_EXIT(status);

    status = _bcm_ppd_frwrd_ipv6_mc_compress_sip_traverse(unit, flags, cb, user_data);
    BCMDNX_IF_ERR_EXIT(status);

exit:
    BCMDNX_FUNC_RETURN;
}

#endif /* INCLUDE_L3 */ 

