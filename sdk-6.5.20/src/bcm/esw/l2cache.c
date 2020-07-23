/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * L2 Cache - Layer 2 BPDU and overflow address cache
 */

#include <sal/core/libc.h>

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/l2u.h>

#include <bcm/l2.h>
#include <bcm/error.h>
#include <bcm_int/common/multicast.h>

#include <bcm_int/esw/port.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/vpn.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/stack.h>
#ifdef BCM_TRIDENT_SUPPORT
#include <bcm_int/esw/trident.h>
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/subport.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */
#ifdef BCM_GREYHOUND2_SUPPORT
#include <bcm_int/esw/greyhound2.h>
#endif

#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/tomahawk3_dispatch.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#ifdef BCM_TRIDENT3_SUPPORT
#include <bcm_int/esw/triumph.h>
#endif
#ifdef BCM_XGS3_SWITCH_SUPPORT

/*
 * Function:
 *      _bcm_l2_cache_gport_resolve
 * Purpose:
 *      Convert destination in a GPORT format to modid port pair or trunk id
 * Parameters:
 *      unit - Unit number
 *      l2caddr - (IN/OUT) Hardware-independent L2 entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_l2_cache_gport_resolve(int unit, bcm_l2_cache_addr_t *l2caddr)
{
    bcm_port_t      port;
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             id;


    if (BCM_GPORT_IS_SET(l2caddr->dest_port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, l2caddr->dest_port, 
                                                   &modid, &port, &tgid, &id));

#if defined(BCM_HGPROXY_COE_SUPPORT)
        if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
            (soc_feature(unit, soc_feature_channelized_switching))) &&
            _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, l2caddr->dest_port)) {
        } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
        if (_BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit,
            l2caddr->dest_port)) {
        } else
#endif
        {
            /* MPLS MIM and WLAN GPORTS are not currently supported. */
            if (-1 != id) {
                return (BCM_E_UNAVAIL);
            }
        }
        /* Trunk GPORT */
        if (BCM_TRUNK_INVALID != tgid) {
            l2caddr->flags |= BCM_L2_CACHE_TRUNK;
            l2caddr->dest_trunk = tgid;
        } else {    /* MODPORT GPORT */
            l2caddr->dest_port = port;
            l2caddr->dest_modid = modid;
        }
    }
    if (BCM_GPORT_IS_SET(l2caddr->src_port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, l2caddr->src_port, 
                                                   &port));
        l2caddr->src_port = port;
    }


    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_l2_cache_gport_construct
 * Purpose:
 *      Constract a GPORT destination format from modid port pair or trunk id
 * Parameters:
 *      unit - Unit number
 *      l2caddr - (IN/OUT) Hardware-independent L2 entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_l2_cache_gport_construct(int unit, bcm_l2_cache_addr_t *l2caddr)
{
    bcm_gport_t         gport;
    _bcm_gport_dest_t   gport_dst;

     _bcm_gport_dest_t_init(&gport_dst);

     if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
         gport_dst.gport_type = BCM_GPORT_TYPE_TRUNK;
         gport_dst.tgid = l2caddr->dest_trunk;
     } else {
         gport_dst.gport_type = BCM_GPORT_TYPE_MODPORT;
         gport_dst.modid = l2caddr->dest_modid;
         gport_dst.port = l2caddr->dest_port;
     }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_construct(unit, &gport_dst, &gport));

    l2caddr->dest_port = gport;

    gport_dst.gport_type = BCM_GPORT_TYPE_MODPORT;
    gport_dst.port = l2caddr->src_port;
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &gport_dst.modid));
    if (gport_dst.modid < 0) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_construct(unit, &gport_dst, &gport));

    l2caddr->src_port = gport;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_l2_from_l2u
 * Purpose:
 *      Convert L2 User table entry to a hardware-independent L2 cache entry.
 * Parameters:
 *      unit - Unit number
 *      l2caddr - (OUT) Hardware-independent L2 entry
 *      l2u_entry - Firebolt L2 User entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_cache_from_l2u(int unit, 
                       bcm_l2_cache_addr_t *l2caddr, l2u_entry_t *l2u_entry)
{
    l2u_entry_t     l2u_mask_entry;
    bcm_module_t    mod_in = 0, mod_out;
    bcm_port_t      port_in = 0, port_out;
    uint32          mask[SOC_MAX_MEM_WORDS];
    int             skip_l2u, isGport, port_field_len, modid_field_len, rval;
    bcm_vlan_t      vlan;
    soc_field_t     port_field = 0;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)	
    int             my_station_config = 0;
#endif
#if defined(BCM_GREYHOUND2_SUPPORT)
    int  l2caddr_l3_is_set = 0;
#endif /* BCM_GREYHOUND2_SUPPORT */

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        if (!_l2u_field32_get(unit, l2u_entry, VALIDf)) {
            return BCM_E_NOT_FOUND;
        }

#if defined(BCM_GREYHOUND2_SUPPORT)
        /* BCM_L2_CACHE_L3 is specified to get the content from MY_STATION_TCAM */
        if (soc_feature(unit, soc_feature_gh2_my_station) &&
            l2caddr->flags & BCM_L2_CACHE_L3) {
            l2caddr_l3_is_set = 1;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */
        sal_memset(l2caddr, 0, sizeof (*l2caddr));
        sal_memset(&l2u_mask_entry, 0, sizeof (l2u_mask_entry));

        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, KEYf)) {
            _l2u_field_get(unit, l2u_entry, MASKf, mask);
            _l2u_field_set(unit, &l2u_mask_entry, KEYf, mask);
        } else {
            _l2u_field_get(unit, l2u_entry, MASKf, (uint32 *)&l2u_mask_entry);
            /* key fields start from bit 1 */
            l2u_mask_entry.entry_data[2] = l2u_mask_entry.entry_data[1] >> 31; 
            l2u_mask_entry.entry_data[1] =
                (l2u_mask_entry.entry_data[1] << 1) | 
                (l2u_mask_entry.entry_data[0] >> 31); 
            l2u_mask_entry.entry_data[0] = l2u_mask_entry.entry_data[0] << 1;
        }

        _l2u_mac_addr_get(unit, l2u_entry, MAC_ADDRf, l2caddr->mac);
        _l2u_mac_addr_get(unit, &l2u_mask_entry, MAC_ADDRf, l2caddr->mac_mask);

        vlan = _l2u_field32_get(unit, l2u_entry, VLAN_IDf);
        l2caddr->vlan_mask = _l2u_field32_get(unit, &l2u_mask_entry, VLAN_IDf);

#if defined(BCM_TRX_SUPPORT) && defined(INCLUDE_L3)
        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, KEY_TYPEf) &&
            _l2u_field32_get(unit, l2u_entry, KEY_TYPEf)) {
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit)) {
                vlan = _l2u_field32_get(unit, l2u_entry, VFIf);
                l2caddr->vlan_mask = _l2u_field32_get(unit, &l2u_mask_entry, VFIf);
                _BCM_VPN_SET(l2caddr->vlan_mask, _BCM_VPN_TYPE_VFI, l2caddr->vlan_mask);
            }
#endif

#ifdef BCM_MPLS_SUPPORT
            if (soc_feature(unit, soc_feature_mpls) &&
               (_bcm_vfi_used_get(unit, vlan, _bcmVfiTypeMpls))) {
                    _BCM_VPN_SET(vlan, _BCM_VPN_TYPE_VFI, vlan);
            } else
#endif /* BCM_MPLS_SUPPORT */
            {
                _BCM_VPN_SET(vlan, _BCM_VPN_TYPE_VFI, vlan);
            }
        }
#endif /* TRX && L3 */
        l2caddr->vlan = vlan;

        l2caddr->prio = _l2u_field32_get(unit, l2u_entry, PRIf);

        if (_l2u_field32_get(unit, l2u_entry, RPEf)) {
            l2caddr->flags |= BCM_L2_CACHE_SETPRI;
        }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if ((SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit)
             || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) &&
            _l2u_field32_get(unit, l2u_entry, DO_NOT_LEARN_MACSAf)) {
            l2caddr->flags |= BCM_L2_CACHE_LEARN_DISABLE;
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

        if (_l2u_field32_get(unit, l2u_entry, CPUf)) {
            l2caddr->flags |= BCM_L2_CACHE_CPU;
        }

        if (_l2u_field32_get(unit, l2u_entry, BPDUf)) {
            l2caddr->flags |= BCM_L2_CACHE_BPDU;
        }

        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, L2_PROTOCOL_PKTf)) {
            if (_l2u_field32_get(unit, l2u_entry, L2_PROTOCOL_PKTf)) {
                l2caddr->flags |= BCM_L2_CACHE_PROTO_PKT;
            }
        }

        if (_l2u_field32_get(unit, l2u_entry, DST_DISCARDf)) {
            l2caddr->flags |= BCM_L2_CACHE_DISCARD;
        }

        if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (soc_feature(unit, soc_feature_generic_dest)) {
                uint32 dest_t = SOC_MEM_FIF_DEST_INVALID;
                uint32 dest_v = soc_mem_field32_dest_get(unit, L2_USER_ENTRYm, 
                                        l2u_entry, DESTINATIONf, &dest_t);
                if (dest_t == SOC_MEM_FIF_DEST_LAG) {
                    l2caddr->flags |= BCM_L2_CACHE_TRUNK;
                    l2caddr->dest_trunk = dest_v & SOC_MEM_FIF_DGPP_TGID_MASK;
                } else if (dest_t == SOC_MEM_FIF_DEST_DGPP) {
                    mod_in = (dest_v & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >> 
                             SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
                    port_in = dest_v & SOC_MEM_FIF_DGPP_PORT_MASK;
                } else {
                    /* XXX */;
                }
            } else {
                if (_l2u_field32_get(unit, l2u_entry, Tf)) {
                    l2caddr->flags |= BCM_L2_CACHE_TRUNK;
                    l2caddr->dest_trunk = _l2u_field32_get(unit, l2u_entry, TGIDf);
                } else {
                    mod_in = _l2u_field32_get(unit, l2u_entry, MODULE_IDf);
                    port_in = _l2u_field32_get(unit, l2u_entry, PORT_NUMf);
                    port_field = PORT_NUMf;
                }
            }
        } else {
            mod_in = _l2u_field32_get(unit, l2u_entry, MODULE_IDf);
            port_in = _l2u_field32_get(unit, l2u_entry, PORT_TGIDf);
            port_field = PORT_TGIDf;
            if (port_in & BCM_TGID_TRUNK_INDICATOR(unit)) {
                l2caddr->flags |= BCM_L2_CACHE_TRUNK;
                l2caddr->dest_trunk = BCM_MODIDf_TGIDf_TO_TRUNK(unit, mod_in,
                                                                port_in);
            }
        }

        if (BCM_MAC_IS_MCAST(l2caddr->mac) &&
            !_BCM_VPN_IS_SET(l2caddr->vlan)) {
            l2caddr->flags |= BCM_L2_CACHE_MULTICAST;
            l2caddr->dest_modid = mod_in;
            l2caddr->dest_port = port_in;
            port_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                  port_field);
            modid_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                   MODULE_IDf);
            /* If MACDA is MC, HW interprets value encoded in MODULE_IDf
               and PORT_FIELD as L2MC_INDEX */
            l2caddr->group = ((port_in & ((1 << port_field_len) - 1)) |
                              ((mod_in & ((1 << modid_field_len) - 1))
                              << port_field_len));
            /* Translate l2mc index */
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType,
                                           &rval));
            if (rval) {
                _BCM_MULTICAST_GROUP_SET(l2caddr->group, _BCM_MULTICAST_TYPE_L2,
                                         l2caddr->group);
            }
        }

        if (!((l2caddr->flags & BCM_L2_CACHE_TRUNK) ||
            (l2caddr->flags & BCM_L2_CACHE_MULTICAST))) {
            if (!SOC_MODID_ADDRESSABLE(unit, mod_in)) {
                return BCM_E_BADID;
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                                       mod_in, port_in, &mod_out,
                                                       &port_out));
            l2caddr->dest_modid = mod_out;
            l2caddr->dest_port = port_out;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

        if (isGport) {
            BCM_IF_ERROR_RETURN(
                _bcm_l2_cache_gport_construct(unit, l2caddr));
        }

        if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f)) {
            if (_l2u_field32_get(unit, l2u_entry, L3f)) {
                l2caddr->flags |= BCM_L2_CACHE_L3;
            }
        } else if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f)) {
            if (_l2u_field32_get(unit, l2u_entry, RESERVED_0f)) {
                l2caddr->flags |= BCM_L2_CACHE_L3;
            }
        }

#if defined(BCM_GREYHOUND2_SUPPORT)
        if (soc_feature(unit, soc_feature_gh2_my_station) && l2caddr_l3_is_set) {
            l2caddr->flags |= BCM_L2_CACHE_L3;
        }
#endif /* BCM_GREYHOUND2_SUPPORT */

        if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, MIRRORf)) {
            if (_l2u_field32_get(unit, l2u_entry, MIRRORf)) {
                l2caddr->flags |= BCM_L2_CACHE_MIRROR;
            }
        }

#if defined(BCM_TRX_SUPPORT) 
        if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, CLASS_IDf)) {
            l2caddr->lookup_class = _l2u_field32_get(unit, l2u_entry, CLASS_IDf);
        }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        my_station_config = soc_property_get(unit, 
                                       spn_L2_ENTRY_USED_AS_MY_STATION, 0);
        if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)) {
            uint32 dest_type = 0;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_mysta_profile)) {
                if (my_station_config != 0) {
                    bcm_l2_station_t station;
                    uint32 mysta_prof_id = 0;
                    my_station_profile_1_entry_t entry_prof;
                    sal_memset(&station, 0, sizeof(bcm_l2_station_t));
                    mysta_prof_id = soc_mem_field32_dest_get(unit, L2_USER_ENTRYm, l2u_entry,
                                    DESTINATIONf, &dest_type);
                    if (dest_type == SOC_MEM_FIF_DEST_MYSTA) {
                        /*MY_STATION_PROFILE_2 is used*/
                        BCM_IF_ERROR_RETURN(_bcm_l2_mysta_profile_entry_get(unit,
                                            &entry_prof, 1, mysta_prof_id));
                        _bcm_l2_mysta_entry_to_station(unit, &entry_prof, MY_STATION_PROFILE_2m, &station);
                        l2caddr->station_flags = station.flags;
                    } else if (dest_type != SOC_MEM_FIF_DEST_INVALID) {
                        l2caddr->station_flags = 0;
                    }
                }
            } else
#endif
            {
                dest_type = _l2u_field32_get(unit, l2u_entry, DEST_TYPEf);

                if ((my_station_config != 0) && (dest_type == 1)) {

                    uint32 dst = _l2u_field32_get(unit, l2u_entry, DESTINATIONf);

                    if((dst & _BCM_L2_DEST_TYPE_MASK) == _BCM_L2_DEST_STATION_HIT){
                        if (dst & _BCM_L2_DEST_STATION_IPV4) {
                            l2caddr->station_flags |= BCM_L2_STATION_IPV4;
                        }

                        if (dst & _BCM_L2_DEST_STATION_IPV6) {
                            l2caddr->station_flags |= BCM_L2_STATION_IPV6;
                        }

                        if (dst & _BCM_L2_DEST_STATION_ARP_RARP) {
                            l2caddr->station_flags |= BCM_L2_STATION_ARP_RARP;
                        }

                        if (dst & _BCM_L2_DEST_STATION_OAM) {
                            l2caddr->station_flags |= BCM_L2_STATION_OAM;
                        }

                        if (dst & _BCM_L2_DEST_STATION_FCOE) {
                            l2caddr->station_flags |= BCM_L2_STATION_FCOE;
                        }

                        if (dst  & _BCM_L2_DEST_STATION_IPV4_MCAST) {
                            l2caddr->station_flags |= BCM_L2_STATION_IPV4_MCAST;
                        }

                        if (dst & _BCM_L2_DEST_STATION_IPV6_MCAST) {
                            l2caddr->station_flags |= BCM_L2_STATION_IPV6_MCAST;
                        }
                    }
                    /* clear fake trunk configuration */
                    if ((dst & _BCM_L2_DEST_TYPE_MASK) != _BCM_L2_DEST_TYPE_TRUNK) {
                        l2caddr->flags &= (~BCM_L2_CACHE_TRUNK);
                        l2caddr->dest_trunk = 0;
                    }
                }
            }
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_fb_l2_to_l2u
 * Purpose:
 *      Convert a hardware-independent L2 cache entry to a L2 User table entry.
 * Parameters:
 *      unit - Unit number
 *      l2u_entry - (OUT) Firebolt L2 User entry
 *      l2caddr - Hardware-independent L2 entry
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_l2_cache_to_l2u(int unit, 
                     l2u_entry_t *l2u_entry, bcm_l2_cache_addr_t *l2caddr)
{
    l2u_entry_t     l2u_mask_entry;
    bcm_module_t    mod_in, mod_out;
    bcm_port_t      port_in, port_out;
    uint32          mask[SOC_MAX_MEM_WORDS];
    soc_field_t     port_field = 0;
    int             skip_l2u, isGport;
    bcm_vlan_t      vlan;
    int             int_pri_max, modid_field_len, port_field_len;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    int             my_station_config = 0;
    uint32          dest_value = 0;
#endif
    int valid_bit = 0;
    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);
    
    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {

        if ((_BCM_VPN_IS_SET(l2caddr->vlan) != _BCM_VPN_IS_SET(l2caddr->vlan_mask)) && (l2caddr->vlan_mask)) {
            return BCM_E_PARAM;
        }

        /* If VPN specified do not perform vlan id check */
        if (!_BCM_VPN_IS_SET(l2caddr->vlan)) {
            VLAN_CHK_ID(unit, l2caddr->vlan);
            VLAN_CHK_ID(unit, l2caddr->vlan_mask);
        }

        if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
            int_pri_max = (SOC_IS_TD_TT(unit) || SOC_IS_KATANAX(unit)) ? 15 : 7;
            if (l2caddr->prio < 0 || l2caddr->prio > int_pri_max) {
                return BCM_E_PARAM;
            }

            if (SOC_IS_TRIDENT3X(unit) &&
                (l2caddr->flags & BCM_L2_CACHE_BPDU) &&
                !(l2caddr->flags & BCM_L2_CACHE_CPU) &&
                !(l2caddr->flags & BCM_L2_CACHE_DISCARD)) {
                LOG_ERROR(BSL_LS_BCM_L2, (BSL_META_U(unit,
                          "L2Cache: SETPRI is not supported "
                          "for BPDU packet in this device, please use FP\n")));
            }
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)){
            my_station_config = soc_property_get(unit, spn_L2_ENTRY_USED_AS_MY_STATION, 0);
            if(my_station_config != 0) {
                if ((l2caddr->station_flags &
                    (~( BCM_L2_STATION_IPV4        |
                        BCM_L2_STATION_IPV6        |
                        BCM_L2_STATION_ARP_RARP    |
                        BCM_L2_STATION_OAM         |
                        BCM_L2_STATION_FCOE        |
                        BCM_L2_STATION_IPV4_MCAST  |
                        BCM_L2_STATION_IPV6_MCAST))) != 0) {
                    return BCM_E_PARAM;
                }
                if (l2caddr->station_flags == 0) {
                    my_station_config = 0;
                }
                else if ((l2caddr->flags & BCM_L2_CACHE_TRUNK) != 0) {
                    return BCM_E_UNAVAIL;
                }
            }
        }
        if ((my_station_config == 0) && (l2caddr->station_flags != 0)) {
            return BCM_E_PARAM;
        }
#endif

        if (BCM_GPORT_IS_SET(l2caddr->dest_port) ||
            BCM_GPORT_IS_SET(l2caddr->src_port)) {
            BCM_IF_ERROR_RETURN(_bcm_l2_cache_gport_resolve(unit, l2caddr));
            isGport = 1;
        } else {
            isGport = 0;
        }

        sal_memset(l2u_entry, 0, sizeof (*l2u_entry));
        sal_memset(&l2u_mask_entry, 0, sizeof (l2u_mask_entry));

        valid_bit = 1 <<  soc_mem_field_length(unit, L2_USER_ENTRYm, VALIDf);
        valid_bit -= 1;
        _l2u_field32_set(unit, l2u_entry, VALIDf, valid_bit);

        vlan = l2caddr->vlan;
        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, KEY_TYPEf)) {
            if (_BCM_VPN_IS_SET(l2caddr->vlan)) {
                _l2u_field32_set(unit, l2u_entry, KEY_TYPEf, 1);
                _BCM_VPN_GET(vlan, _BCM_VPN_TYPE_VFI, l2caddr->vlan);
            }
            _l2u_field32_set(unit, &l2u_mask_entry, KEY_TYPEf, 1);

            if (l2caddr->flags & BCM_L2_CACHE_PROTO_PKT) {
                if (soc_mem_field_valid(unit, L2_USER_ENTRYm, 
                                        L2_PROTOCOL_PKTf)) {
                    _l2u_field32_set(unit, l2u_entry, L2_PROTOCOL_PKTf, 1);
                } else {
                  return BCM_E_UNAVAIL;
                }
            }

        } else {
            if (_BCM_VPN_IS_SET(l2caddr->vlan)) {
                return BCM_E_CONFIG;
            }
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (_BCM_VPN_IS_SET(l2caddr->vlan) &&
            (SOC_IS_TD2P_TT2P(unit) || SOC_IS_APACHE(unit))) {
            _BCM_VPN_GET(vlan, _BCM_VPN_TYPE_VFI, l2caddr->vlan);
            _l2u_field32_set(unit, l2u_entry, VFIf, vlan);
            if(l2caddr->vlan_mask != 0) {
                _BCM_VPN_GET(vlan, _BCM_VPN_TYPE_VFI, l2caddr->vlan_mask);
                _l2u_field32_set(unit, &l2u_mask_entry, VFIf, vlan);
            }
        }
        else
#endif
        {
            _l2u_field32_set(unit, l2u_entry, VLAN_IDf, vlan);
            _l2u_field32_set(unit, &l2u_mask_entry, VLAN_IDf, l2caddr->vlan_mask);
        }
        _l2u_mac_addr_set(unit, l2u_entry, MAC_ADDRf, l2caddr->mac);
        _l2u_mac_addr_set(unit, &l2u_mask_entry, MAC_ADDRf, l2caddr->mac_mask);

        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, KEYf)) {
            _l2u_field_get(unit, &l2u_mask_entry, KEYf, mask);
        } else {
            /* key fields start from bit 1 */
            mask[0] = (l2u_mask_entry.entry_data[1] << 31) | 
                (l2u_mask_entry.entry_data[0] >> 1); 
            mask[1] = (l2u_mask_entry.entry_data[2] << 31) | 
                (l2u_mask_entry.entry_data[1] >> 1); 
        }
        _l2u_field_set(unit, l2u_entry, MASKf, mask);

        if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
            _l2u_field32_set(unit, l2u_entry, PRIf, l2caddr->prio);
            _l2u_field32_set(unit, l2u_entry, RPEf, 1);
        }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT)
        if ((SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit)
             || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) &&
            (l2caddr->flags & BCM_L2_CACHE_LEARN_DISABLE)) {
            _l2u_field32_set(unit, l2u_entry, DO_NOT_LEARN_MACSAf, 1);
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAVEN_SUPPORT */

        if (l2caddr->flags & BCM_L2_CACHE_CPU) {
            _l2u_field32_set(unit, l2u_entry, CPUf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_BPDU) {
            _l2u_field32_set(unit, l2u_entry, BPDUf, 1);
        }

        if (l2caddr->flags & BCM_L2_CACHE_DISCARD) {
            _l2u_field32_set(unit, l2u_entry, DST_DISCARDf, 1);
        }

        if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
            if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
                if (soc_feature(unit, soc_feature_generic_dest)) {
                    soc_mem_field32_dest_set(unit, L2_USER_ENTRYm, l2u_entry,
                        DESTINATIONf, SOC_MEM_FIF_DEST_LAG, l2caddr->dest_trunk);
                } else {
                    _l2u_field32_set(unit, l2u_entry, Tf, 1);
                    _l2u_field32_set(unit, l2u_entry, TGIDf, l2caddr->dest_trunk);
                }
            } else {
                port_field = PORT_NUMf;
            }
        } else {
            if (SOC_IS_TOMAHAWKX(unit)) {
                /*
                 * implies the switch has a latency mode enabled. trunking is
                 * not supported in any latency mode in TH.
                 */
                port_field = PORT_NUMf;
            } else {
                if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
                    _l2u_field32_set(unit, l2u_entry, MODULE_IDf,
                                     BCM_TRUNK_TO_MODIDf(unit,
                                                         l2caddr->dest_trunk));
                    _l2u_field32_set(unit, l2u_entry, PORT_TGIDf,
                                     BCM_TRUNK_TO_TGIDf(unit,
                                                        l2caddr->dest_trunk));
                } else {
                    port_field = PORT_TGIDf;
                }
            }
        }

        if (!((l2caddr->flags & BCM_L2_CACHE_TRUNK) ||
            (l2caddr->flags & BCM_L2_CACHE_MULTICAST))) {
            mod_in = l2caddr->dest_modid;
            port_in = l2caddr->dest_port;
            if (!isGport) {
            PORT_DUALMODID_VALID(unit, port_in);
            }
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                        mod_in, port_in, &mod_out, &port_out));
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return BCM_E_BADID;
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return BCM_E_PORT;
            }

            if (soc_feature(unit, soc_feature_generic_dest)) {
                soc_mem_field32_dest_set(unit, L2_USER_ENTRYm, l2u_entry,
                    DESTINATIONf, SOC_MEM_FIF_DEST_DGPP,
                    mod_out << SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS | port_out);
            } else {
                _l2u_field32_set(unit, l2u_entry, MODULE_IDf, mod_out);
                _l2u_field32_set(unit, l2u_entry, port_field, port_out);
            }
        }

        if ((l2caddr->flags & BCM_L2_CACHE_MULTICAST) &&
            !_BCM_VPN_IS_SET(l2caddr->vlan)) {
            if (_BCM_MULTICAST_IS_SET(l2caddr->group)) {
                if (_BCM_MULTICAST_IS_L2(l2caddr->group)) {
                    port_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                          port_field);
                    modid_field_len = soc_mem_field_length(unit, L2_USER_ENTRYm,
                                                           MODULE_IDf);
                    /* If MACDA is MC, HW interprets value encoded in MODULE_IDf
                       and PORT_FIELD as L2MC_INDEX */
                    _l2u_field32_set(unit, l2u_entry, port_field,
                                     (_BCM_MULTICAST_ID_GET(l2caddr->group) &
                                     ((1 << port_field_len) - 1)));
                    _l2u_field32_set(unit, l2u_entry, MODULE_IDf,
                                     ((_BCM_MULTICAST_ID_GET(l2caddr->group) >>
                                     port_field_len) &
                                     ((1 << modid_field_len) - 1)));
                } else {
                    return BCM_E_PARAM;
                }
            }
        }

        if (l2caddr->flags & BCM_L2_CACHE_L3) {
            if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f)) {
                _l2u_field32_set(unit, l2u_entry, L3f, 1);
            } else if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm,
                                           RESERVED_0f)) {
                _l2u_field32_set(unit, l2u_entry, RESERVED_0f, 1);
            }
        }

        if (l2caddr->flags & BCM_L2_CACHE_MIRROR) {
            if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, MIRRORf)) {
                _l2u_field32_set(unit, l2u_entry, MIRRORf, 1);
            }
        }

#if defined(BCM_TRX_SUPPORT) 
        if (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, CLASS_IDf)) {
            _l2u_field32_set(unit, l2u_entry, CLASS_IDf, l2caddr->lookup_class);
        }
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (my_station_config != 0 && soc_feature(unit, soc_feature_l2_entry_used_as_my_station)) {
#if defined(BCM_TRIDENT3_SUPPORT)
            if (soc_feature(unit, soc_feature_mysta_profile)) {
                uint32 mysta_profile_id;
                bcm_l2_station_t station;
                my_station_profile_2_entry_t entry;

                sal_memset(&station, 0, sizeof(bcm_l2_station_t));
                sal_memset(&entry, 0, sizeof(entry));
                station.flags = l2caddr->station_flags;
                if (l2caddr->station_flags != 0) {
                    _bcm_l2_mysta_station_to_entry(unit, &station, MY_STATION_PROFILE_2m, &entry);
                    BCM_IF_ERROR_RETURN(
                        _bcm_l2_mysta_profile_entry_add(unit, &entry, 1,
                                                        &mysta_profile_id));
                    soc_mem_field32_dest_set(unit, L2_USER_ENTRYm, l2u_entry,
                        DESTINATIONf, SOC_MEM_FIF_DEST_MYSTA, mysta_profile_id);
                }
            } else
#endif
            {
                _l2u_field32_set(unit, l2u_entry, DEST_TYPEf, 1);

                dest_value = _BCM_L2_DEST_STATION_HIT;

                if (l2caddr->station_flags & BCM_L2_STATION_IPV4) {
                    dest_value |= _BCM_L2_DEST_STATION_IPV4;
                }

                if (l2caddr->station_flags & BCM_L2_STATION_IPV6) {
                    dest_value |= _BCM_L2_DEST_STATION_IPV6;
                }

                if (l2caddr->station_flags & BCM_L2_STATION_ARP_RARP) {
                    dest_value |= _BCM_L2_DEST_STATION_ARP_RARP;
                }

                if (l2caddr->station_flags & BCM_L2_STATION_OAM) {
                    dest_value |= _BCM_L2_DEST_STATION_OAM;
                }

                if (l2caddr->station_flags & BCM_L2_STATION_FCOE) {
                    dest_value |= _BCM_L2_DEST_STATION_FCOE;
                }

                if (l2caddr->station_flags & BCM_L2_STATION_IPV4_MCAST) {
                    dest_value |= _BCM_L2_DEST_STATION_IPV4_MCAST;
                }

                if (l2caddr->station_flags & BCM_L2_STATION_IPV6_MCAST) {
                    dest_value |= _BCM_L2_DEST_STATION_IPV6_MCAST;
                }
                _l2u_field32_set(unit, l2u_entry, DESTINATIONf, dest_value);
            }
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Function:
 *      bcm_esw_l2_cache_init
 * Purpose:
 *      Initialize the L2 cache
 * Parameters:
 *      unit - device number
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Clears all entries and preloads a few BCM_L2_CACHE_BPDU
 *      entries to match previous generation of devices.
 */
int
bcm_esw_l2_cache_init(int unit)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int     skip_l2u;
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    uint64  regval64;
    int     my_station_config = 0;
#endif

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_cache_init(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_cache_init(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        bcm_l2_cache_addr_t addr;
        l2u_entry_t entry;
        int index;

        if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
            SOC_IF_ERROR_RETURN
                (soc_mem_clear(unit, L2_USER_ENTRYm, COPYNO_ALL, TRUE));
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        COMPILER_64_ZERO(regval64);
        /* Enable L2 entry used as my station hit */
        my_station_config = soc_property_get(unit,
                                       spn_L2_ENTRY_USED_AS_MY_STATION, 0);
        if (soc_feature(unit, soc_feature_l2_entry_used_as_my_station)) {
            SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &regval64));
            if (my_station_config != soc_reg64_field32_get(unit, ING_CONFIG_64r,
                regval64, L2_ENTRY_USED_AS_MY_STATIONf)) {
                BCM_IF_ERROR_RETURN
                   (soc_reg_field32_modify(unit, ING_CONFIG_64r, REG_PORT_ANY,
                             L2_ENTRY_USED_AS_MY_STATIONf, my_station_config));
            }
        }
#endif

        bcm_l2_cache_addr_t_init(&addr);
        addr.flags = BCM_L2_CACHE_CPU | BCM_L2_CACHE_BPDU;
        /* Set default BPDU addresses (01:80:c2:00:00:00) */
        ENET_SET_MACADDR(addr.mac, _soc_mac_spanning_tree);
        ENET_SET_MACADDR(addr.mac_mask, _soc_mac_all_ones);
        BCM_IF_ERROR_RETURN (bcm_esw_stk_my_modid_get(unit, &addr.dest_modid));
        addr.dest_port = CMIC_PORT(unit);
        BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
        SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));

        if (soc_mem_field_valid(unit, L2_USER_ENTRYm, L2_PROTOCOL_PKTf)) {
            soc_mem_field32_set(unit, L2_USER_ENTRYm,
                                      &entry, L2_PROTOCOL_PKTf, 1);
            if (!soc_feature(unit, soc_feature_l2_no_vfi)) {
                soc_mem_field32_set(unit, L2_USER_ENTRYm, &entry, KEY_TYPEf, 1);
            }
            if (soc_mem_field_valid(unit, L2_USER_ENTRYm, VFI_MASKf)) {
               soc_mem_field32_set(unit, L2_USER_ENTRYm, &entry, VFI_MASKf, 0);
            }
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
        }

        if (!SAL_BOOT_QUICKTURN && !SAL_BOOT_XGSSIM) {
            /* Set 01:80:c2:00:00:10 */
            addr.mac[5] = 0x10;
            BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
            /* Set 01:80:c2:00:00:0x */
            addr.mac[5] = 0x00;
            addr.mac_mask[5] = 0xf0;
            BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
            /* Set 01:80:c2:00:00:2x */
            addr.mac[5] = 0x20;
            BCM_IF_ERROR_RETURN(_bcm_l2_cache_to_l2u(unit, &entry, &addr));
            SOC_IF_ERROR_RETURN(soc_l2u_insert(unit, &entry, -1, &index));
        }

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *      bcm_esw_l2_cache_size_get
 * Purpose:
 *      Get number of L2 cache entries
 * Parameters:
 *      unit - device number
 *      size - (OUT) number of entries in cache
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_size_get(int unit, int *size)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_cache_size_get(unit, size);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        *size = soc_mem_index_count(unit, L2_USER_ENTRYm);

        return 0;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_set
 * Purpose:
 *      Set an L2 cache entry
 * Parameters:
 *      unit - device number
 *      index - l2 cache entry number (or -1)
 *              If -1 is given then the entry used will be the first
 *              available entry if the l2 cache address mac_mask
 *              field is all ones, or the last available entry if
 *              the mac_mask field has some zeros.  Cache entries
 *              are matched from lowest entry to highest entry.
 *      addr - l2 cache address
 *      index_used - (OUT) l2 cache entry number actually used
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_set(int unit, int index, bcm_l2_cache_addr_t *addr,
                 int *index_used)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;
    l2u_entry_t l2u_entry;
    bcm_l2_cache_addr_t l2_addr;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_cache_set(unit, index, addr, index_used);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_cache_set(unit, index, addr, index_used);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        if (index < -1 || index > soc_mem_index_max(unit, L2_USER_ENTRYm)) {
            return BCM_E_PARAM;
        }

        sal_memcpy(&l2_addr, addr, sizeof(bcm_l2_cache_addr_t));

        if (soc_feature(unit, soc_feature_overlaid_address_class)) {
            if ((l2_addr.lookup_class > SOC_OVERLAID_ADDR_CLASS_MAX(unit)) ||
                (l2_addr.lookup_class < 0)) {
                return (BCM_E_PARAM);
            }
        } else if ((l2_addr.lookup_class > SOC_ADDR_CLASS_MAX(unit)) || 
                   (l2_addr.lookup_class < 0)) {
            return (BCM_E_PARAM);
        }

        /* ESW architecture can't support multiple destinations */
        if (BCM_PBMP_NOT_NULL(l2_addr.dest_ports)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(
            _bcm_l2_cache_to_l2u(unit, &l2u_entry, &l2_addr));
        
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) &&
            (addr->flags & BCM_L2_CACHE_L3)) {
            int rv;
            if (index == -1) {
                rv = bcm_td_l2cache_myStation_lookup(unit, &l2_addr, &index);
                if (BCM_FAILURE(rv)) {
                    /* find the first free index in L2_USER_ENTRY */
                    BCM_IF_ERROR_RETURN
                        (soc_l2u_find_free_entry(unit, &l2u_entry, &index));
                }
            }

            rv = bcm_td_l2cache_myStation_set(unit, index, &l2_addr);
        }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */

        SOC_IF_ERROR_RETURN
            (soc_l2u_insert(unit, &l2u_entry, index, index_used));

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_get
 * Purpose:
 *      Get an L2 cache entry
 * Parameters:
 *      unit - device number
 *      index - l2 cache entry index
 *      size - (OUT) l2 cache address
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_get(int unit, int index, bcm_l2_cache_addr_t *addr)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u;
    l2u_entry_t l2u_entry;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_cache_get(unit, index, addr);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT */

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    L2_INIT(unit)

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_cache_get(unit, index, addr);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        if (index < 0 || index > soc_mem_index_max(unit, L2_USER_ENTRYm)) {
            return BCM_E_PARAM;
        }

        SOC_IF_ERROR_RETURN(
            soc_l2u_get(unit, &l2u_entry, index));

        BCM_IF_ERROR_RETURN(
            _bcm_l2_cache_from_l2u(unit, addr, &l2u_entry));

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) &&
            (addr->flags & BCM_L2_CACHE_L3)) {
            BCM_IF_ERROR_RETURN
                (bcm_td_l2cache_myStation_get(unit, index, addr));
        }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */

        return BCM_E_NONE;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_delete
 * Purpose:
 *      Clear an L2 cache entry
 * Parameters:
 *      unit - device number
 *      index - l2 cache entry index
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_delete(int unit, int index)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u, rv;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_cache_delete(unit, index);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_cache_delete(unit, index);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        if (index < 0 || index > soc_mem_index_max(unit, L2_USER_ENTRYm)) {
            return BCM_E_PARAM;
        }

        rv = BCM_E_NONE;
        soc_mem_lock(unit, L2_USER_ENTRYm);
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm)) {
            l2u_entry_t l2u_entry;
            rv = soc_l2u_get(unit, &l2u_entry, index);
            if (BCM_SUCCESS(rv))  {
                if ((SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, L3f) &&
                     _l2u_field32_get(unit, &l2u_entry, L3f)) ||
                    (SOC_MEM_FIELD_VALID(unit, L2_USER_ENTRYm, RESERVED_0f) &&
                     _l2u_field32_get(unit, &l2u_entry, RESERVED_0f))) {
                     rv = bcm_td_l2cache_myStation_delete(unit, index);
                } else if (SOC_IS_KATANAX(unit) || 
                    soc_feature(unit, soc_feature_my_station_tcam_check) ||
                    soc_feature(unit, soc_feature_gh2_my_station)) {
	                   /* Katana doesn't has L3,RESERVED_0 fields
                              so ignoring return value ! */
                           bcm_td_l2cache_myStation_delete(unit, index);
                } /* Nothing can be done..!! */
            }
        }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */
        if (BCM_SUCCESS(rv)) {
            rv = soc_l2u_delete(unit, NULL, index, &index);
        }
        soc_mem_unlock(unit, L2_USER_ENTRYm);
        return rv;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_l2_cache_delete_all
 * Purpose:
 *      Clear all L2 cache entries
 * Parameters:
 *      unit - device number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_l2_cache_delete_all(int unit)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int skip_l2u, rv, index, index_max;
    l2u_entry_t entry;

#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* Added to handle SDK functions calling esw version,
     * instead of Tomahawk3's version
     */
    if (SOC_IS_TOMAHAWK3(unit)) {
        return bcm_tomahawk3_l2_cache_delete_all(unit);
    }
#endif /* BCM_TOMAHAWK3_SUPPORT  */

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    L2_INIT(unit);

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_ism_memory)) {
        return bcm_tr3_l2_cache_delete_all(unit);
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

    skip_l2u = soc_property_get(unit, spn_SKIP_L2_USER_ENTRY, 0);

    if (soc_feature(unit, soc_feature_l2_user_table) && !skip_l2u) {
        index_max = soc_mem_index_max(unit, L2_USER_ENTRYm);

        rv = BCM_E_NONE;
        soc_mem_lock(unit, L2_USER_ENTRYm);
        for (index = 0; index <= index_max; index++) {
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
            if (soc_mem_is_valid(unit, MY_STATION_TCAMm)) {
                rv = READ_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &entry);
                if (BCM_SUCCESS(rv) &&
                    (index <= soc_mem_index_max(unit, MY_STATION_TCAMm))) {
                    rv = bcm_td_l2cache_myStation_delete(unit, index);
                    /* Ignore entry not found case */
                    if (rv == BCM_E_NOT_FOUND) {
                        rv = BCM_E_NONE;
                    }
                }
            }
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */
            if (BCM_SUCCESS(rv)) {
                sal_memset(&entry, 0, sizeof(entry));
                rv = WRITE_L2_USER_ENTRYm(unit, MEM_BLOCK_ALL, index, &entry);
            }
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        soc_mem_unlock(unit, L2_USER_ENTRYm);
        return rv;
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}
