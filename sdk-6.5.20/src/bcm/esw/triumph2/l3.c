/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3.c
 * Purpose:     Triumph2 L3 function implementations
 */


#include <soc/defs.h>

#include <assert.h>
#include <shared/bsl.h>
#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH2_SUPPORT)  && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>
#include <soc/defragment.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT*/
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2_SUPPORT*/
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#include <bcm_int/esw/ecn.h>

#define ECN_INFO(_unit_)   (&_bcm_xgs5_ecn_bk_info[_unit_])

#define BSL_LSS_SCACHE (BSL_LS_SHARED_SCACHE | BSL_VERBOSE)

/* L3 ECMP table defragmentation buffer info */
typedef struct _bcm_tr2_l3_ecmp_defragment_buffer_s {
    int base_ptr; /* Starting index of the buffer used
                     in ECMP table defragmentation */
    int size;     /* Size of the buffer */
} _bcm_tr2_l3_ecmp_defragment_buffer_t;

extern int ecmp_mode_hierarchical;
extern int ecmp_mode_single;

STATIC _bcm_tr2_l3_ecmp_defragment_buffer_t
       *_bcm_tr2_l3_ecmp_defragment_buffer_info[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_tr2_l3_tnl_term_add
 * Purpose:
 *      Add tunnel termination entry to the hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      tnl_info - (IN)Tunnel terminator parameters. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_tnl_term_add(int unit, uint32 *entry_ptr, bcm_tunnel_terminator_t *tnl_info)
{
    bcm_module_t mod_in, mod_out, my_mod;
    bcm_port_t port_in, port_out;
    _bcm_l3_ingress_intf_t iif;
    int tunnel, wlan;
    int rv;

    /* Program remote port */
    if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) || 
        (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
        (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
        (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) {
        wlan = 1;
    } else {
        wlan = 0;
    }

    if (wlan) {
        if (tnl_info->flags & BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE) {
            if (!BCM_GPORT_IS_MODPORT(tnl_info->remote_port)) {
                return BCM_E_PARAM;
            }
            mod_in = BCM_GPORT_MODPORT_MODID_GET(tnl_info->remote_port);
            port_in = BCM_GPORT_MODPORT_PORT_GET(tnl_info->remote_port); 
            BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, 
                                        port_in, &mod_out, &port_out));
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return (BCM_E_BADID);
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return (BCM_E_PORT);
            }
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
        } else {
            /* Send to the local loopback port */
            rv = bcm_esw_stk_my_modid_get(unit, &my_mod);
            BCM_IF_ERROR_RETURN(rv);
            port_in = 54;
            mod_in = my_mod;
            rv = _bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, 
                                        port_in, &mod_out, &port_out);
            BCM_IF_ERROR_RETURN(rv);

            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
        }
        /* Program tunnel id */
        if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
            if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
                return BCM_E_PARAM;
            }
            if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                tunnel = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_IDf,
                                           tunnel);
            }
        }

    }
    if ((tnl_info->type == bcmTunnelTypeAutoMulticast) ||
        (tnl_info->type == bcmTunnelTypeAutoMulticast6)) {
        /* Program L3_IIFm */
        if(SOC_MEM_FIELD_VALID(unit, L3_IIFm, IPMC_L3_IIFf)) {
            sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
            iif.intf_id = tnl_info->vlan;

            rv = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
            BCM_IF_ERROR_RETURN(rv);
            iif.vrf = tnl_info->vrf;
#if defined(BCM_TRIDENT2_SUPPORT)
            if (soc_feature(unit, soc_feature_l3_iif_profile)) {
                iif.profile_flags |= _BCM_L3_IIF_PROFILE_DO_NOT_UPDATE;
            }
#endif /* BCM_TRIDENT2_SUPPORT */
            rv = _bcm_tr_l3_ingress_interface_set(unit, &iif,NULL, NULL);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Program tunnel id */
        if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
            if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
                return BCM_E_PARAM;
            }
            if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, TUNNEL_IDf)) {
                tunnel = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
                soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_IDf,
                                           tunnel);
            }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_l3_tnl_term_entry_init
 * Purpose:
 *      Initialize soc tunnel terminator entry key portion.
 * Parameters:
 *      unit     - (IN)  BCM device number. 
 *      tnl_info - (IN)  BCM buffer with tunnel info.
 *      entry    - (OUT) SOC buffer with key filled in.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_tnl_term_entry_init(int unit, bcm_tunnel_terminator_t *tnl_info,
                                soc_tunnel_term_t *entry)
{
    int       idx;                /* Entry iteration index.     */
    int       idx_max;            /* Entry widht.               */
    uint32    *entry_ptr;         /* Filled entry pointer.      */
    _bcm_tnl_term_type_t tnl_type;/* Tunnel type.               */
    int       rv;                 /* Operation return status.   */
    int valid_bit = 0;
    soc_mem_t mem = L3_TUNNELm;

    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        mem = L3_TUNNEL_DOUBLEm;
    }

    /* Get tunnel type & sub_type. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_set_tnl_term_type(unit, tnl_info, &tnl_type));

    /* Reset destination structure. */
    sal_memset(entry, 0, sizeof(soc_tunnel_term_t));

    /* Set Destination/Source pair. */
    entry_ptr = (uint32 *)&entry->entry_arr[0];
    if (tnl_type.tnl_outer_hdr_ipv6 == 1) {
        /* Apply mask on source address. */
        rv = bcm_xgs3_l3_mask6_apply(tnl_info->sip6_mask, tnl_info->sip6);
        BCM_IF_ERROR_RETURN(rv);

        /* SIP [0-63] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[0], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP [64-127] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[1], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP [0-63] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[2], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP [64-127] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[3], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_UPPER_ONLY);

        /* SIP MASK [0-63] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[0], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP MASK [64-127] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[1], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP MASK [0-63] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[2], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP MASK [64-127] */
        soc_mem_ip6_addr_set(unit, mem,
                             (uint32 *)&entry->entry_arr[3], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_UPPER_ONLY);
    }  else if (tnl_type.tnl_outer_hdr_ipv6 == 0) {
        tnl_info->sip &= tnl_info->sip_mask;

        /* Set destination ip. */
        soc_mem_field32_set(unit, mem, entry_ptr, DIPf, tnl_info->dip);

        /* Set source ip. */
        soc_mem_field32_set(unit, mem, entry_ptr, SIPf, tnl_info->sip);

        /* Set destination subnet mask. */
        soc_mem_field32_set(unit, mem, entry_ptr, DIP_MASKf,
                                   tnl_info->dip_mask);

        /* Set source subnet mask. */
        soc_mem_field32_set(unit, mem, entry_ptr, SIP_MASKf,
                                   tnl_info->sip_mask);
    }
    /* Resolve number of entries hw entry occupies. */
    idx_max = (tnl_type.tnl_outer_hdr_ipv6 == 1) ? SOC_TNL_TERM_IPV6_ENTRY_WIDTH : \
              (tnl_type.tnl_outer_hdr_ipv6 == 0) ? SOC_TNL_TERM_IPV4_ENTRY_WIDTH : 0; 

    for (idx = 0; idx < idx_max; idx++) {
        entry_ptr = (uint32 *)&entry->entry_arr[idx];

        /* Set valid bit. */
        valid_bit = 1 <<  soc_mem_field_length(unit,mem,VALIDf);
        valid_bit -= 1;
        soc_mem_field32_set(unit, mem, entry_ptr, VALIDf, valid_bit);

        /* Set tunnel subtype. */
        soc_mem_field32_set(unit, mem, entry_ptr, SUB_TUNNEL_TYPEf,
                                   tnl_type.tnl_sub_type);

        /* Set tunnel type. */
        soc_mem_field32_set(unit, mem, entry_ptr, TUNNEL_TYPEf,
                                   tnl_type.tnl_auto);

        /* Set entry mode. */

    if (SOC_MEM_FIELD_VALID(unit, mem, MODEf)) {
        soc_mem_field32_set(unit, mem, entry_ptr, MODEf,
                                   tnl_type.tnl_outer_hdr_ipv6);
        soc_mem_field32_set(unit, mem, entry_ptr, MODE_MASKf, 1);
    } else if (SOC_MEM_FIELD_VALID(unit, mem, KEY_TYPEf)) {
        soc_mem_field32_set(unit, mem, entry_ptr, KEY_TYPEf,
                tnl_type.tnl_outer_hdr_ipv6);
        soc_mem_field32_set(unit, mem, entry_ptr, KEY_TYPE_MASKf, 1);
    }

        if (0 == idx) {
            /* Set the PROTOCOL field */
            if ((tnl_info->type == bcmTunnelTypeIpAnyIn4) || 
                (tnl_info->type == bcmTunnelTypeIpAnyIn6))
            {
              /* Set PROTOCOL and PROTOCOL_MASK field to zero for IpAnyInx tunnel type*/
               soc_mem_field32_set(unit, mem, entry_ptr, PROTOCOLf, 0x0);
               soc_mem_field32_set(unit, mem, entry_ptr, PROTOCOL_MASKf, 0x0);
            } else {            
                soc_mem_field32_set(unit, mem, entry_ptr, PROTOCOLf,
                                           tnl_type.tnl_protocol);
                soc_mem_field32_set(unit, mem, entry_ptr,
                                           PROTOCOL_MASKf, 0xff);
            }
        }
        
        if (SOC_MEM_FIELD_VALID(unit, mem, BFD_ENABLEf)) {
            soc_mem_field32_set(unit, mem, entry_ptr, BFD_ENABLEf, 0);
        }

        if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) ||
            (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
            (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
            (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) { 

            /* Set the L4 ports - WLAN/AMT tunnels */
            if (0 == idx) {
                soc_mem_field32_set(unit, mem, entry_ptr, L4_DEST_PORTf,
                                           tnl_info->udp_dst_port);

                soc_mem_field32_set(unit, mem, entry_ptr,
                                           L4_DEST_PORT_MASKf, 0xffff);

                soc_mem_field32_set(unit, mem, entry_ptr, L4_SRC_PORTf,
                                           tnl_info->udp_src_port);
    
                soc_mem_field32_set(unit, mem, entry_ptr,
                                           L4_SRC_PORT_MASKf, 0xffff);
            }

            /* Set UDP tunnel type. */
            if (soc_mem_field_valid(unit, mem, UDP_TUNNEL_TYPEf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, UDP_TUNNEL_TYPEf,
                                           tnl_type.tnl_udp_type);
            }
            /* Ignore UDP checksum */
            if (soc_mem_field_valid(unit, mem, IGNORE_UDP_CHECKSUMf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, IGNORE_UDP_CHECKSUMf, 0x1);
            }
        } else if (tnl_info->type == bcmTunnelTypeAutoMulticast) {

            /* Set UDP tunnel type. */
            if (soc_mem_field_valid(unit, mem, UDP_TUNNEL_TYPEf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, UDP_TUNNEL_TYPEf,
                                           tnl_type.tnl_udp_type);
            }

            if (soc_mem_field_valid(unit, mem, IGNORE_UDP_CHECKSUMf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, IGNORE_UDP_CHECKSUMf, 0x1);
            }

            if (soc_mem_field_valid(unit, mem, CTRL_PKTS_TO_CPUf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, CTRL_PKTS_TO_CPUf, 0x1);
            }
        } else if (tnl_info->type == bcmTunnelTypeAutoMulticast6) {

            /* Set UDP tunnel type. */
            if (soc_mem_field_valid(unit, mem, UDP_TUNNEL_TYPEf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, UDP_TUNNEL_TYPEf,
                                           tnl_type.tnl_udp_type);
            }

            if (soc_mem_field_valid(unit, mem, IGNORE_UDP_CHECKSUMf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, IGNORE_UDP_CHECKSUMf, 0x1);
            }

            if (soc_mem_field_valid(unit, mem, CTRL_PKTS_TO_CPUf)) {
                soc_mem_field32_set(unit, mem, entry_ptr, CTRL_PKTS_TO_CPUf, 0x1);
            }
        }

        /* Save vlan id for ipmc lookup.*/
        if((tnl_info->vlan) && SOC_MEM_FIELD_VALID(unit, mem, L3_IIFf)) {
            soc_mem_field32_set(unit, mem, entry_ptr, L3_IIFf, tnl_info->vlan);
        }

        /* Set GRE payload */
        if (tnl_type.tnl_gre) {
            /* GRE IPv6 payload is allowed. */
            soc_mem_field32_set(unit, mem, entry_ptr, GRE_PAYLOAD_IPV6f,
                                       tnl_type.tnl_gre_v6_payload);

            /* GRE IPv6 payload is allowed. */
            soc_mem_field32_set(unit, mem, entry_ptr, GRE_PAYLOAD_IPV4f,
                                       tnl_type.tnl_gre_v4_payload);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_l3_tnl_term_entry_parse
 * Purpose:
 *      Parse tunnel terminator entry portion.
 * Parameters:
 *      unit     - (IN)  BCM device number. 
 *      entry    - (IN)  SOC buffer with tunne information.  
 *      tnl_info - (OUT) BCM buffer with tunnel info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_tnl_term_entry_parse(int unit, soc_tunnel_term_t *entry,
                                  bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_tnl_term_type_t tnl_type;     /* Tunnel type information.   */
    uint32 *entry_ptr;                 /* Filled entry pointer.      */
    int tunnel_id;                     /* Tunnel ID */
    int remote_port;                   /* Remote port */
    bcm_module_t mod, mod_out, my_mod; /* Module IDs */
    bcm_port_t port, port_out;         /* Physical ports */
    soc_mem_t mem = L3_TUNNELm;
    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination structure. */
    bcm_tunnel_terminator_t_init(tnl_info);
    sal_memset(&tnl_type, 0, sizeof(_bcm_tnl_term_type_t));

    if (soc_feature(unit, soc_feature_tunnel_term_hash_table)) {
        mem = L3_TUNNEL_DOUBLEm;
    }

    entry_ptr = (uint32 *)&entry->entry_arr[0];

    /* Get valid bit. */
    if (!soc_mem_field32_get(unit, mem, entry_ptr, VALIDf)) {
        return (BCM_E_NOT_FOUND);
    }

    if (SOC_MEM_FIELD_VALID(unit, mem, MODEf)) {
        tnl_type.tnl_outer_hdr_ipv6 =
            soc_mem_field32_get(unit, mem, entry_ptr, MODEf);
    } else if (SOC_MEM_FIELD_VALID(unit, mem, KEY_TYPEf)) {
        tnl_type.tnl_outer_hdr_ipv6 =
            soc_mem_field32_get(unit, mem, entry_ptr, KEY_TYPEf);

    }
     
    /* Get Destination/Source pair. */
    if (tnl_type.tnl_outer_hdr_ipv6 == 1) {
        /* SIP [0-63] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[0], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP [64-127] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[1], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP [0-63] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[2], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP [64-127] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[3], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_UPPER_ONLY);

        /* SIP MASK [0-63] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[0], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP MASK [64-127] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[1], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP MASK [0-63] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[2], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP MASK [64-127] */
        soc_mem_ip6_addr_get(unit, mem,
                             (uint32 *)&entry->entry_arr[3], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_UPPER_ONLY);

    }  else if (tnl_type.tnl_outer_hdr_ipv6 == 0) {
        /* Get destination ip. */
        tnl_info->dip = soc_mem_field32_get(unit, mem, entry_ptr, DIPf);

        /* Get source ip. */
        tnl_info->sip = soc_mem_field32_get(unit, mem, entry_ptr, SIPf);

        /* Destination subnet mask. */
        tnl_info->dip_mask = BCM_XGS3_L3_IP4_FULL_MASK;

        /* Source subnet mask. */
        tnl_info->sip_mask = soc_mem_field32_get(unit, mem, entry_ptr,
                                                        SIP_MASKf);
    }

    /* Get tunnel subtype. */
    tnl_type.tnl_sub_type =
        soc_mem_field32_get(unit, mem, entry_ptr, SUB_TUNNEL_TYPEf);

    /* Get UDP tunnel type. */
    if (soc_mem_field_valid(unit, mem, UDP_TUNNEL_TYPEf)) {
        tnl_type.tnl_udp_type =
            soc_mem_field32_get(unit, mem, entry_ptr, UDP_TUNNEL_TYPEf);
    }

    /* Get tunnel type. */
    tnl_type.tnl_auto =
        soc_mem_field32_get(unit, mem, entry_ptr, TUNNEL_TYPEf);

    /* Copy DSCP from outer header flag. */
    if (soc_mem_field32_get(unit, mem, entry_ptr, USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }
    /* Copy TTL from outer header flag. */
    if (soc_mem_field32_get(unit, mem, entry_ptr, USE_OUTER_HDR_TTLf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }
    /* Keep inner header DSCP flag. */
    if (soc_mem_field32_get(unit, mem, entry_ptr,
                                   DONOT_CHANGE_INNER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    }

    soc_mem_pbmp_field_get(unit, mem, entry_ptr, ALLOWED_PORT_BITMAPf,
                           &tnl_info->pbmp);

    /* Tunnel or IPMC lookup vlan id */
    tnl_info->vlan = soc_mem_field32_get(unit, mem, entry_ptr, IINTFf);

    /*  Get trust dscp per tunnel */ 
    if (soc_mem_field32_get(unit, mem, entry_ptr, USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_DSCP_TRUST;
    }
    
    /* Get Tunnel class id for VFP match */
    if(SOC_MEM_FIELD_VALID(unit, mem, TUNNEL_CLASS_IDf)) {
        tnl_info->tunnel_class = soc_mem_field32_get(unit, mem, entry_ptr,
                                    TUNNEL_CLASS_IDf);
    }

    /* Get the protocol field and make some decisions */
    tnl_type.tnl_protocol = soc_mem_field32_get(unit, mem, entry_ptr,
                                                       PROTOCOLf);
    switch (tnl_type.tnl_protocol) {
        case 0x2F:
            tnl_type.tnl_gre = 1;
            break;
        case 0x67:
            tnl_type.tnl_pim_sm = 1;
        default:
            break;
    }
    /* Get gre IPv4 payload allowed. */
    tnl_type.tnl_gre_v4_payload = 
        soc_mem_field32_get(unit, mem, entry_ptr, GRE_PAYLOAD_IPV4f);

    /* Get gre IPv6 payload allowed. */
    tnl_type.tnl_gre_v6_payload = 
        soc_mem_field32_get(unit, mem, entry_ptr, GRE_PAYLOAD_IPV6f);

    /* Get the L4 data */
    if (soc_mem_field_valid (unit, mem, L4_SRC_PORTf)) {
        tnl_info->udp_src_port = soc_mem_field32_get(unit, mem, entry_ptr,
                                                        L4_SRC_PORTf);
    }
    if (soc_mem_field_valid (unit, mem, L4_DEST_PORTf)) {
        tnl_info->udp_dst_port = soc_mem_field32_get(unit, mem, entry_ptr,
                                                        L4_DEST_PORTf);
    }
    /* Get the tunnel ID */
    if (SOC_MEM_FIELD_VALID(unit, mem, TUNNEL_IDf)) {
        tunnel_id = soc_mem_field32_get(unit, mem, entry_ptr, TUNNEL_IDf);
        if (tunnel_id) {
            BCM_GPORT_TUNNEL_ID_SET(tnl_info->tunnel_id, tunnel_id);
            tnl_info->flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
        }
    }

    /* Get tunnel type, sub_type and protocol. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_get_tnl_term_type(unit, tnl_info, &tnl_type));
    /* Get the remote port member */
    if (SOC_MEM_FIELD_VALID(unit, mem, REMOTE_TERM_GPPf)) {
        remote_port = soc_mem_field32_get(unit, mem, entry_ptr,
                                             REMOTE_TERM_GPPf);
        mod = (remote_port >> 6) & 0x7F;
        port = remote_port & 0x3F;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                    mod, port, &mod_out, &port_out));
        BCM_GPORT_MODPORT_SET(tnl_info->remote_port, mod_out, port_out);
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_mod));
        if (mod != my_mod) {
            if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) ||
                (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
                (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
                (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) {
                tnl_info->flags |= BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE;
            }
        }
    }
    /* Get IIF if l3 ingress mode is not set */
    if (!BCM_XGS3_L3_INGRESS_MODE_ISSET(unit) &&
        BCM_XGS3_L3_ING_IF_TBL_SIZE(unit) && (tnl_info->vlan > BCM_VLAN_MAX)) {
        _bcm_l3_ingress_intf_t iif;
        sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
        iif.intf_id = tnl_info->vlan;
        BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, NULL, &iif));

        tnl_info->vrf = iif.vrf;
        tnl_info->if_class = iif.if_class;
        tnl_info->qos_map_id = iif.qos_map_id;
#ifdef BCM_TOMAHAWK_SUPPORT        
        if (SOC_IS_TOMAHAWKX(unit) && soc_feature(unit, soc_feature_ecn_wred)) {
            if (bcmi_xgs5_ecn_map_used_get(unit, iif.tunnel_term_ecn_map_id,
                                           _bcmEcnmapTypeTunnelTerm)) {
                tnl_info->tunnel_term_ecn_map_id = iif.tunnel_term_ecn_map_id |
                    _BCM_XGS5_ECN_MAP_TYPE_TUNNEL_TERM;
                tnl_info->flags |= BCM_TUNNEL_TERM_ECN_MAP;
            }
        }
#endif        
    }
#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, mem, DEFAULT_VLAN_PROFILE_INDEXf)) {
        int vlan_profile_idx = soc_mem_field32_get(unit, mem, entry_ptr,
                                             DEFAULT_VLAN_PROFILE_INDEXf);
        if (vlan_profile_idx) {
            /* Get the forwarding VLAN ID */
            tunnel_default_vlan_profile_entry_t tunnel_default_vlan_profile_entry;

            BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_TUNNEL_DEFAULT_VLAN_PROFILEm,
                                 MEM_BLOCK_ANY, vlan_profile_idx,
                                 &tunnel_default_vlan_profile_entry));

            tnl_info->fwd_vlan = soc_mem_field32_get(unit, L3_TUNNEL_DEFAULT_VLAN_PROFILEm,
                                     (uint32 *)&tunnel_default_vlan_profile_entry, OUTER_VLAN_TAGf);
        }
    }
#endif

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_l3_ecmp_grp_get
 * Purpose:
 *      Get ecmp group next hop members by index.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp - (IN)Ecmp group id to read. 
 *      ecmp_count - (IN)Maximum number of entries to read.
 *      nh_idx     - (OUT)Next hop indexes. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_ecmp_grp_get(int unit, int ecmp_grp, int ecmp_group_size, int *nh_idx)
{
    int idx;                                /* Iteration index.              */
    int max_ent_count;                      /* Number of entries to read.    */
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry.      */
    int one_entry_grp = TRUE;               /* Single next hop entry group.  */ 
    int rv = BCM_E_UNAVAIL;                 /* Operation return status.      */
    int ecmp_idx;

    /* Input parameters sanity check. */
    if ((NULL == nh_idx) || (ecmp_group_size < 1)) {
        return (BCM_E_PARAM);
    }

    /* Zero all next hop indexes first. */
    sal_memset(nh_idx, 0, ecmp_group_size * sizeof(int));
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Get group base pointer. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm,
                                          MEM_BLOCK_ANY, ecmp_grp, hw_buf));
    ecmp_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, hw_buf, BASE_PTRf);

    /* Read zero based ecmp count. */
    rv = soc_mem_read(unit, L3_ECMP_COUNTm, MEM_BLOCK_ANY, ecmp_grp, hw_buf);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    max_ent_count = soc_mem_field32_get(unit, L3_ECMP_COUNTm, hw_buf, COUNTf);
    max_ent_count++; /* Count is zero based. */ 

    /* Read all the indexes from hw. */
    for (idx = 0; idx < max_ent_count; idx++) {

        /* Read next hop index. */
        rv = soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY, 
                          (ecmp_idx + idx), hw_buf);
        if (rv < 0) {
            break;
        }
        nh_idx[idx] = soc_mem_field32_get(unit, L3_ECMPm, 
                                          hw_buf, NEXT_HOP_INDEXf);
        /* Check if group contains . */ 
        if (idx && (nh_idx[idx] != nh_idx[0])) { 
            one_entry_grp = FALSE;
        }

        if (0 == soc_feature(unit, soc_feature_l3_dynamic_ecmp_group)) {
            /* Next hops popuplated in cycle,stop once you read first entry again */
            if (idx && (FALSE == one_entry_grp) && (nh_idx[idx] == nh_idx[0])) {
                nh_idx[idx] = 0;
                break;
            }
        } else {
             one_entry_grp = FALSE;
        }
    }
    /* Reset rest of the group if only 1 next hop is present. */
    if (one_entry_grp) {
       sal_memset(nh_idx + 1, 0, (ecmp_group_size - 1) * sizeof(int)); 
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_l3_ecmp_grp_add
 * Purpose:
 *      Add ecmp group next hop members, or reset ecmp group entry.  
 *      NOTE: Function always writes all the entries in ecmp group.
 *            If there is not enough nh indexes - next hops written
 *            in cycle. 
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp   - (IN)ecmp group id to write.
 *      buf        - (IN)Next hop indexes or NULL for entry reset.
 *      info       - (IN)ECMP additional info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_ecmp_grp_add(int unit, int ecmp_grp, void *buf, void *info)
{
    uint32 l3_ecmp[SOC_MAX_MEM_FIELD_WORDS];        /* l3_ecmp buf             */
    uint32 l3_ecmp_count[SOC_MAX_MEM_FIELD_WORDS];  /* l3_ecmp_count buf       */
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];         /* l3_ecmp_count buf       */
    int ecmp_idx;                                   /* Ecmp table entry index. */
    int *nh_idx;                                    /* Ecmp group nh indexes.  */
    int nh_cycle_idx;
    int entry_type;
    uint32 reg_val, value;
    ing_l3_next_hop_entry_t ing_nh;
    _bcm_l3_tbl_op_t data;
    int max_grp_size = 0;                           /* Maximum ecmp group size.*/
    int idx = 0;                                    /* Iteration index.        */
    int rv = BCM_E_UNAVAIL;                         /* Operation return value. */
    int l3_ecmp_oif_flds[8] = {  L3_OIF_0f, 
                             L3_OIF_1f, 
                             L3_OIF_2f, 
                             L3_OIF_3f, 
                             L3_OIF_4f, 
                             L3_OIF_5f, 
                             L3_OIF_6f, 
                             L3_OIF_7f }; 
    int l3_ecmp_oif_type_flds[8] = {  L3_OIF_0_TYPEf, 
                             L3_OIF_1_TYPEf, 
                             L3_OIF_2_TYPEf, 
                             L3_OIF_3_TYPEf, 
                             L3_OIF_4_TYPEf, 
                             L3_OIF_5_TYPEf, 
                             L3_OIF_6_TYPEf, 
                             L3_OIF_7_TYPEf };
    _bcm_l3_ecmp_group_info_t *ecmp_info = info;
    _bcm_l3_ecmp_group_buffer_t *ecmp_buffer = buf;

    /* Input parameters check. */
    if (NULL == ecmp_buffer || NULL == ecmp_info ||
        NULL == ecmp_buffer->group_buffer) {
        return (BCM_E_PARAM);
    }

    /* Cast input buffer. */
    nh_idx = ecmp_buffer->group_buffer;
    max_grp_size = ecmp_info->max_paths;

    if (BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit,
                                ecmp_grp), ecmp_grp)) {
        /* Group has already exists, get base ptr from group table */ 
        sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm,
                            MEM_BLOCK_ANY, ecmp_grp, hw_buf));
        ecmp_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, hw_buf, BASE_PTRf);
    } else {  
        /* Get index to the first slot in the ECMP table
         * that can accomodate max_grp_size */
        data.width = ecmp_info->max_paths;
        data.tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp); 
        data.oper_flags = _BCM_L3_SHR_TABLE_TRAVERSE_CONTROL; 
        data.entry_index = -1;
        rv = _bcm_xgs3_tbl_free_idx_get(unit, &data);
        if (BCM_FAILURE(rv)) {
            if (rv == BCM_E_FULL) {
                /* Defragment ECMP table */
                BCM_IF_ERROR_RETURN(bcm_tr2_l3_ecmp_defragment_no_lock(unit, 0));
    
                /* Attempt to get free index again */
                BCM_IF_ERROR_RETURN(_bcm_xgs3_tbl_free_idx_get(unit, &data));
            } else {
                return rv;
            }
        }
        ecmp_idx = data.entry_index;
        BCM_XGS3_L3_ENT_REF_CNT_INC(data.tbl_ptr, data.entry_index, max_grp_size);
    }

    if (ecmp_idx >= BCM_XGS3_L3_ECMP_TBL_SIZE(unit)) {
        return BCM_E_FULL;
    }

    sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Write all the indexes to hw. */
    for (idx = 0, nh_cycle_idx = 0; idx < max_grp_size; idx++, nh_cycle_idx++) {

        /* Set next hop index. */
        sal_memset (l3_ecmp, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

        /* If this is the last nhop then program black-hole. */
        if ( (!idx) && (!nh_idx[nh_cycle_idx]) ) {
              nh_cycle_idx = 0;
        } else  if (!nh_idx[nh_cycle_idx]) {
            break;
        }

        soc_mem_field32_set(unit, L3_ECMPm, l3_ecmp, NEXT_HOP_INDEXf,
                            nh_idx[nh_cycle_idx]);

        /* Write buffer to hw L3_ECMPm table. */
        rv = soc_mem_write(unit, L3_ECMPm, MEM_BLOCK_ALL, 
                           (ecmp_idx + idx), l3_ecmp);
        if (BCM_FAILURE(rv)) {
            break;
        }

        /* Write buffer to hw INITIAL_L3_ECMPm table. */
        rv = soc_mem_write(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL, 
                                    (ecmp_idx + idx), l3_ecmp);
        if (BCM_FAILURE(rv)) {
            break;
        }
        
        if (soc_feature(unit, soc_feature_urpf)) {
            /* Check if URPF is enabled on device */
            BCM_IF_ERROR_RETURN(
                 READ_L3_DEFIP_RPF_CONTROLr(unit, &reg_val));
            if (reg_val) {
                if (idx < 8) {
                    BCM_IF_ERROR_RETURN (soc_mem_read(unit, 
                       ING_L3_NEXT_HOPm, MEM_BLOCK_ANY, 
                       nh_idx[idx], &ing_nh));

                    entry_type = soc_ING_L3_NEXT_HOPm_field32_get(unit, 
                                              &ing_nh, ENTRY_TYPEf);

                    if (entry_type == 0x0) {
                        value = soc_ING_L3_NEXT_HOPm_field32_get(unit, 
                                      &ing_nh, VLAN_IDf);
                        if (soc_mem_field_valid(unit, 
                                L3_ECMP_COUNTm, l3_ecmp_oif_type_flds[idx])) {
                            soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                      l3_ecmp_oif_type_flds[idx], entry_type);
                        }
                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                      l3_ecmp_oif_flds[idx], value);
                    } else if (entry_type == 0x1) {
                        value  = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, L3_OIFf);
                        if (soc_mem_field_valid(unit, 
                                L3_ECMP_COUNTm, l3_ecmp_oif_type_flds[idx])) {
                            soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                      l3_ecmp_oif_type_flds[idx], entry_type);
                        }
                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                        l3_ecmp_oif_flds[idx], value);
                    }
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, URPF_COUNTf, idx);
                } else {
                    /* Inorder to avoid TRAP_TO_CPU, urpf_mode on L3_IIF/PORT must be set 
                     *  to STRICT_MODE / LOOSE_MODE */
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, ECMP_GT8f , 1);
                }
            }
        }
    }

    if (BCM_SUCCESS(rv)) {
        /* mode 0 = 512 ecmp groups */
        if (!BCM_XGS3_L3_MAX_ECMP_MODE(unit)) { 
            /* Set Max Group Size. */
            sal_memset (l3_ecmp_count, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
            soc_mem_field32_set(unit, L3_ECMP_COUNTm, l3_ecmp_count, COUNTf, max_grp_size - 1);

            rv = soc_mem_write(unit, L3_ECMP_COUNTm, MEM_BLOCK_ALL, 
                               (ecmp_grp+1), l3_ecmp_count);
            BCM_IF_ERROR_RETURN(rv);
        }
        /* Set zero based ecmp count. */
        if (!idx) {
            soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNTf, idx);
        } else {
            soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNTf, idx - 1);
        }

        /* Set group base pointer. */
        soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                            BASE_PTRf, ecmp_idx);

        rv = soc_mem_write(unit, L3_ECMP_COUNTm, MEM_BLOCK_ALL, 
                           ecmp_grp, hw_buf);

        BCM_IF_ERROR_RETURN(rv);

        rv = soc_mem_write(unit, INITIAL_L3_ECMP_COUNTm,
                           MEM_BLOCK_ALL, ecmp_grp, hw_buf);
        /* mode 1 = max possible ecmp groups */
        if (BCM_XGS3_L3_MAX_ECMP_MODE(unit)) {
            /* Save the max possible paths for this ECMP group in s/w */ 
            BCM_XGS3_L3_MAX_PATHS_PERGROUP_PTR(unit)[ecmp_grp] = ecmp_info->max_paths;
        }
    }
    return rv;
}

/*
 * Function:
 *      _bcm_tr2_l3_ecmp_grp_del
 * Purpose:
 *      Reset ecmp group next hop members
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp   - (IN)ecmp group id to write.
 *      info       - (IN)ECMP additional info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_ecmp_grp_del(int unit, int ecmp_grp, void *info)
{
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry.*/
    ecmp_count_entry_t ecmp_count_entry;
    int ecmp_idx;               /* Ecmp table entry index. */
    int idx;                    /* Iteration index.        */
    int rv = BCM_E_UNAVAIL;     /* Operation return value. */
    _bcm_l3_tbl_op_t data;
    data.tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp); 

    if (NULL == info) {
        return BCM_E_INTERNAL;
    }

    /* Initialize ecmp entry. */
    sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Calculate table index. */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit, L3_ECMP_COUNTm,
                MEM_BLOCK_ANY, ecmp_grp, &ecmp_count_entry));
    ecmp_idx = soc_mem_field32_get(unit, L3_ECMP_COUNTm, &ecmp_count_entry,
            BASE_PTRf);

    /* Write all the indexes to hw. */
    for (idx = 0; idx < ((_bcm_l3_ecmp_group_info_t*)info)->max_paths; idx++) {
        /* Write buffer to hw. */
        rv = soc_mem_write(unit, L3_ECMPm, MEM_BLOCK_ALL, 
                           (ecmp_idx + idx), hw_buf);

        if (BCM_FAILURE(rv)) {
            return rv;
        }

        /* Write initial ecmp table. */
        if (SOC_MEM_IS_VALID(unit, INITIAL_L3_ECMPm)) {
            /* Write buffer to hw. */
            rv = soc_mem_write(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL, 
                           (ecmp_idx + idx), hw_buf);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    /* Decrement ref count for the entries in ecmp table
     * Ref count for ecmp_group table is decremented in common table del func. */
    BCM_XGS3_L3_ENT_REF_CNT_DEC(data.tbl_ptr, ecmp_idx,
                        ((_bcm_l3_ecmp_group_info_t*)info)->max_paths);

    /* Set group base pointer. */
    ecmp_idx = ecmp_grp;

    rv = soc_mem_write(unit, L3_ECMP_COUNTm, MEM_BLOCK_ALL, 
                       ecmp_idx, hw_buf);
    BCM_IF_ERROR_RETURN(rv);

    if (!BCM_XGS3_L3_MAX_ECMP_MODE(unit)) {
        rv = soc_mem_write(unit, L3_ECMP_COUNTm, MEM_BLOCK_ALL, 
                (ecmp_idx+1), hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    rv = soc_mem_write(unit, INITIAL_L3_ECMP_COUNTm, MEM_BLOCK_ALL, 
                       ecmp_idx, hw_buf);
    
    if (BCM_XGS3_L3_MAX_ECMP_MODE(unit)) {
        /* Reset the max paths of the deleted group */
        BCM_XGS3_L3_MAX_PATHS_PERGROUP_PTR(unit)[ecmp_grp] = 0;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_tr2_l3_intf_urpf_default_route_set
 * Purpose:
 *      Set urpf_default_route enable flag for the L3 interface. 
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      enable    - (IN)Enable. 
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr2_l3_intf_urpf_default_route_set(int unit, bcm_vlan_t vid, int enable)  
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/
    int ret_val;                /* Operation return value.  */

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;
    soc_mem_lock(unit, L3_IIFm);

    ret_val = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
    if (BCM_FAILURE(ret_val)) {
        soc_mem_unlock(unit, L3_IIFm);
        return (ret_val);
    }

    if (enable) {
       iif.flags |= BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK;
    } else {
       iif.flags &= ~BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK;
    }

    ret_val = _bcm_tr_l3_ingress_interface_set(unit, &iif, NULL, NULL);

    soc_mem_unlock(unit, L3_IIFm);

    return (ret_val);
}

/*
 * Function:
 *      _bcm_tr2_l3_intf_urpf_default_route_get
 * Purpose:
 *      Get the urpf_default_route_check enable flag for the specified L3 interface
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      enable    - (OUT)enable. 
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr2_l3_intf_urpf_default_route_get (int unit, bcm_vlan_t vid, int *enable)
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;

    BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, NULL, &iif));

    *enable = (iif.flags & BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK) ? 1 : 0;
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_tr2_l3_intf_urpf_mode_set
 * Purpose:
 *      Set the urpf_mode info for the specified L3 interface
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      urpf_mode       - (IN)urpf_mode. 
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr2_l3_intf_urpf_mode_set(int unit, bcm_vlan_t vid, bcm_vlan_urpf_mode_t urpf_mode)  
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/
    int ret_val;                /* Operation return value.  */

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }
   
    iif.intf_id = vid;
    soc_mem_lock(unit, L3_IIFm);

    ret_val = _bcm_tr_l3_ingress_interface_get(unit, NULL, &iif);
    if (BCM_FAILURE(ret_val)) {
        soc_mem_unlock(unit, L3_IIFm);
        return (ret_val);
    }

    iif.urpf_mode = urpf_mode;

    ret_val = _bcm_tr_l3_ingress_interface_set(unit, &iif, NULL, NULL);

    soc_mem_unlock(unit, L3_IIFm);

    return (ret_val);
}

/*
 * Function:
 *      _bcm_tr2_l3_intf_urpf_mode_get
 * Purpose:
 *      Get the urpf_mode info for the specified L3 interface
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      vid       - (IN)Vlan id. 
 *      urpf_mode       - (OUT) urpf_mode
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr2_l3_intf_urpf_mode_get(int unit, bcm_vlan_t vid, bcm_vlan_urpf_mode_t *urpf_mode)
{
    _bcm_l3_ingress_intf_t iif; /* Ingress interface config.*/

    /* Input parameters sanity check. */
    if ((vid > soc_mem_index_max(unit, L3_IIFm)) || 
        (vid < soc_mem_index_min(unit, L3_IIFm))) {
        return (BCM_E_PARAM);
    }

    iif.intf_id = vid;

    BCM_IF_ERROR_RETURN(_bcm_tr_l3_ingress_interface_get(unit, NULL, &iif));

    *urpf_mode = iif.urpf_mode;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_buffer_init
 * Purpose:
 *      Initialize L3 ECMP table defragmentation buffer.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_tr2_l3_ecmp_defragment_buffer_init(int unit)
{
    if (NULL == _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]) {
        _bcm_tr2_l3_ecmp_defragment_buffer_info[unit] =
            sal_alloc(sizeof(_bcm_tr2_l3_ecmp_defragment_buffer_t),
                    "l3 ecmp defragmentation buffer info");
        if (NULL == _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]) {
            return BCM_E_MEMORY;
        }
    }
    sal_memset(_bcm_tr2_l3_ecmp_defragment_buffer_info[unit], 0,
            sizeof(_bcm_tr2_l3_ecmp_defragment_buffer_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_buffer_deinit
 * Purpose:
 *      De-initialize L3 ECMP table defragmentation buffer.
 * Parameters:
 *      unit - (IN) SOC unit number.
 * Returns:
 *      None.
 */
void
bcm_tr2_l3_ecmp_defragment_buffer_deinit(int unit)
{
    if (NULL != _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]) {
        sal_free(_bcm_tr2_l3_ecmp_defragment_buffer_info[unit]);
        _bcm_tr2_l3_ecmp_defragment_buffer_info[unit] = NULL;
    }
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_buffer_set
 * Purpose:
 *      Set L3 ECMP table defragmentation buffer.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      size - (IN) Defragmentation buffer size.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_tr2_l3_ecmp_defragment_buffer_set(int unit, int size)
{
    _bcm_l3_tbl_t *tbl_ptr;
    int old_base_ptr, old_size;
    int base_ptr;
    int i, j;
    int buffer_found;

    if (!soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTRf) &&
        !soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTR_0f)) {
        return BCM_E_UNAVAIL;
    }

    L3_LOCK(unit);

    tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp); 

    /* Free old defragmentation buffer */
    old_base_ptr = _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->base_ptr;
    old_size = _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->size;
    if (old_size > 0) {
        BCM_XGS3_L3_ENT_REF_CNT_DEC(tbl_ptr, old_base_ptr, old_size);
    }

    /* Fist, attempt to find new defragmentation buffer at end of ECMP table.
     * Such a buffer introduces less fragmentation than a buffer that's
     * located in the middle of the ECMP table.
     */
    for (i = tbl_ptr->idx_max + 1 - size; i < (tbl_ptr->idx_max + 1); i++) {
        if (BCM_XGS3_L3_ENT_REF_CNT(tbl_ptr, i)) {
            break;
        }
    }

    if (i == (tbl_ptr->idx_max + 1)) {
        /* An available buffer is found at end of ECMP table */
        base_ptr = tbl_ptr->idx_max + 1 - size;
    } else {
        /* Attempt to find new defragmentation buffer in ECMP table */
        buffer_found = FALSE;
        for (i = tbl_ptr->idx_min; i < (tbl_ptr->idx_max + 1 - size); i++) {
            for (j = i; j < (i + size); j++) {
                if (BCM_XGS3_L3_ENT_REF_CNT(tbl_ptr, j)) {
                    break;
                }
            }
            if (j == (i + size)) {
                buffer_found = TRUE;
                break;
            }
        }

        if (buffer_found) {
            base_ptr = i;
        } else {
            /* A free buffer of the requested size cannot be found.
             * Restore the old buffer.
             */
            if (old_size > 0) {
                BCM_XGS3_L3_ENT_REF_CNT_INC(tbl_ptr, old_base_ptr, old_size);
            }

            L3_UNLOCK(unit);
            return BCM_E_RESOURCE;
        }
    }

    BCM_XGS3_L3_ENT_REF_CNT_INC(tbl_ptr, base_ptr, size);
    _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->base_ptr = base_ptr;
    _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->size = size;

    L3_UNLOCK(unit);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_buffer_get
 * Purpose:
 *      Get L3 ECMP table defragmentation buffer size.
 * Parameters:
 *      unit - (IN)  SOC unit number.
 *      size - (OUT) Defragmentation buffer size.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_tr2_l3_ecmp_defragment_buffer_get(int unit, int *size)
{
    if (!soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTRf) &&
        !soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTR_0f)) {
        return BCM_E_UNAVAIL;
    }

    (*size) = _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_l3_ecmp_member_copy
 * Purpose:
 *      Copy ECMP table entry.
 * Parameters:
 *      unit - (IN)  SOC unit number.
 *      src_index - (IN) Index of the entry to be copied.
 *      dst_index - (IN) Destination index.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr2_l3_ecmp_member_copy(int unit, int src_index, int dst_index)
{
    ecmp_entry_t ecmp_entry;
    initial_l3_ecmp_entry_t initial_ecmp_entry;

    if (src_index < 0 || src_index > soc_mem_index_max(unit, L3_ECMPm)) {
        return BCM_E_PARAM;
    }
    if (dst_index < 0 || dst_index > soc_mem_index_max(unit, L3_ECMPm)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, src_index, &ecmp_entry));
    SOC_IF_ERROR_RETURN
        (WRITE_L3_ECMPm(unit, MEM_BLOCK_ALL, dst_index, &ecmp_entry));

    if (SOC_MEM_IS_VALID(unit, INITIAL_L3_ECMPm)) {
        SOC_IF_ERROR_RETURN
            (READ_INITIAL_L3_ECMPm(unit, MEM_BLOCK_ANY, src_index,
                                   &initial_ecmp_entry));
        SOC_IF_ERROR_RETURN
            (WRITE_INITIAL_L3_ECMPm(unit, MEM_BLOCK_ALL, dst_index,
                                    &initial_ecmp_entry));
    }
    /* Increment reference count for ECMP table entry at dst_index */
    BCM_XGS3_L3_ENT_REF_CNT_INC(BCM_XGS3_L3_TBL_PTR(unit, ecmp), dst_index, 1);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_l3_ecmp_member_clear
 * Purpose:
 *      Clear an ECMP table entry.
 * Parameters:
 *      unit  - (IN)  SOC unit number.
 *      index - (IN) Index of the entry to be cleared.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr2_l3_ecmp_member_clear(int unit, int index)
{
    if (index < 0 || index > soc_mem_index_max(unit, L3_ECMPm)) {
        return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(WRITE_L3_ECMPm(unit, MEM_BLOCK_ALL, index,
                soc_mem_entry_null(unit, L3_ECMPm)));
    if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
        SOC_IF_ERROR_RETURN(WRITE_INITIAL_L3_ECMPm(unit, MEM_BLOCK_ALL, index,
                    soc_mem_entry_null(unit, INITIAL_L3_ECMPm)));
    }

    /* Decrement reference count for the just cleared ECMP table entry */
    BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp), index, 1);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_l3_ecmp_group_base_ptr_update
 * Purpose:
 *      Update the base pointer field of the given ECMP group.
 * Parameters:
 *      unit     - (IN) SOC unit number.
 *      group    - (IN) ECMP group.
 *      base_ptr - (IN) New base pointer.
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_tr2_l3_ecmp_group_base_ptr_update(int unit, int group, int base_ptr)
{
    ecmp_count_entry_t ecmp_group_entry;
    uint32 initial_ecmp_group_entry[SOC_MAX_MEM_FIELD_WORDS];
    soc_mem_t initial_ecmp_group_mem;
    soc_field_t base_ptr_f = INVALIDf;

    if (group < 0 || group > soc_mem_index_max(unit, L3_ECMP_COUNTm)) {
        return BCM_E_PARAM;
    }

    /* Read L3 ECMP group table */
    SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, group,
                &ecmp_group_entry));
    
    /* Choose correct base pointer field */
    if (soc_feature(unit, soc_feature_l3_ecmp_1k_groups)) {
        uint32 ing_config_2;
        uint8 ecmp_hash_16bits = 1;
        if (SOC_REG_IS_VALID(unit, ING_CONFIG_2r)) {
            SOC_IF_ERROR_RETURN(
                READ_ING_CONFIG_2r(unit, &ing_config_2));
            ecmp_hash_16bits =
                soc_reg_field_get(unit, ING_CONFIG_2r, ing_config_2,
                                  ECMP_HASH_16BITSf);
        }

        if (ecmp_hash_16bits && 
            soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTRf)) {
            base_ptr_f = BASE_PTRf;
        } else if (soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTR_0f)) {
            base_ptr_f = BASE_PTR_0f;
        }
    } else if (soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTR_0f)) {
        base_ptr_f = BASE_PTR_0f;
    } else {
        base_ptr_f = BASE_PTRf;
    }

    /* Modify base pointer */
    if (base_ptr_f == BASE_PTRf) {
        soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry,
                BASE_PTRf, base_ptr);
    } else {
        soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry,
                BASE_PTR_0f, base_ptr);
        soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry,
                BASE_PTR_1f, base_ptr);
        soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry,
                BASE_PTR_2f, base_ptr);
        soc_L3_ECMP_COUNTm_field32_set(unit, &ecmp_group_entry,
                BASE_PTR_3f, base_ptr);
    }

    /* Write L3 ECMP group table */
    SOC_IF_ERROR_RETURN(WRITE_L3_ECMP_COUNTm(unit, MEM_BLOCK_ALL, group,
                &ecmp_group_entry));

#ifdef BCM_TOMAHAWK3_SUPPORT
    if (soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
        return BCM_E_NONE;
    }
#endif

    /* Read Initial L3 ECMP group table */
    if (soc_mem_is_valid(unit, INITIAL_L3_ECMP_COUNTm)) {
        initial_ecmp_group_mem = INITIAL_L3_ECMP_COUNTm;
    } else if (soc_mem_is_valid(unit, INITIAL_L3_ECMP_GROUPm)) {
        initial_ecmp_group_mem = INITIAL_L3_ECMP_GROUPm;
    } else {
        return BCM_E_INTERNAL;
    }
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, initial_ecmp_group_mem,
                MEM_BLOCK_ANY, group, &initial_ecmp_group_entry));

    /* Modify base pointer */
    if (base_ptr_f == BASE_PTRf) {
        soc_mem_field32_set(unit, initial_ecmp_group_mem,
                &initial_ecmp_group_entry, BASE_PTRf, base_ptr);
    } else {
        soc_mem_field32_set(unit, initial_ecmp_group_mem,
                &initial_ecmp_group_entry, BASE_PTR_0f, base_ptr);
        soc_mem_field32_set(unit, initial_ecmp_group_mem,
                &initial_ecmp_group_entry, BASE_PTR_1f, base_ptr);
        soc_mem_field32_set(unit, initial_ecmp_group_mem,
                &initial_ecmp_group_entry, BASE_PTR_2f, base_ptr);
        soc_mem_field32_set(unit, initial_ecmp_group_mem,
                &initial_ecmp_group_entry, BASE_PTR_3f, base_ptr);
    }

    /* Write Initial L3 ECMP group table */
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, initial_ecmp_group_mem,
                MEM_BLOCK_ALL, group, &initial_ecmp_group_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_no_lock
 * Purpose:
 *      Defragment L3 ECMP table. This procedure does not obtain
 *      L3 lock. It's assumed that L3 lock was obtained before
 *      invoking this procedure. 
 * Parameters:
 *      unit - (IN)  SOC unit number.
 *      ecmp_level - (IN) Ecmp Level
 *                   Default value is 0, entire ECMP table is de-fragmented
 *                   ecmp_level 1 and 2 is applicable to Hierarchical Ecmp feature,
 *                   only level 1 or level 2 Ecmp table is defragmented
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_tr2_l3_ecmp_defragment_no_lock(int unit, int ecmp_level)
{
    _bcm_l3_tbl_t *ecmp_group_tbl_ptr;
    soc_defragment_block_t *block_array;
    soc_defragment_block_t reserved_block;
    soc_defragment_member_op_t member_op;
    soc_defragment_group_op_t group_op;
    int block_count;
    int i, ecmp_group_idx_increment;
    int rv;
    int max_paths;
    soc_field_t base_ptr_f;
    int base_ptr = 0;
    ecmp_count_entry_t ecmp_group_entry;
    int min_idx = 0;
    int max_idx = 0;
    int ecmp_tbl_size = 0;
    int free_base_ptr = 0;
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_RIOT_SUPPORT) || \
    defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    int vp_lag_base_idx = 0;
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
    int hecmp_enable = 0;
    int ecmp_min_idx = 0;
    int ecmp_max_idx = 0;
#endif
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    _bcm_l3_tbl_t *ecmp_tbl_ptr;
#endif

    if (!soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTRf) &&
        !soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTR_0f)) {
        return BCM_E_UNAVAIL;
    }

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (soc_feature(unit, soc_feature_hierarchical_ecmp) &&
       (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode == ecmp_mode_hierarchical)) {
        hecmp_enable = 1;
    }
#endif

    ecmp_group_tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp_grp);

    if (ecmp_level == 1) {
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
                block_array = sal_alloc(sizeof(soc_defragment_block_t) *
                     (ecmp_group_tbl_ptr->split_maxused + 1),
                     "defragment block array");
            } else
#endif
            {
                block_array = sal_alloc(sizeof(soc_defragment_block_t) *
                        (ecmp_group_tbl_ptr->idx_maxused -
                         ecmp_group_tbl_ptr->split_idx + 1),
                        "defragment block array");
            }
        } else
#endif
        {
            /*
             * Allocate block array of size equal to size of overlay table
             * since we do not keep track of the highest used index at Level 1
             */
            block_array = sal_alloc(sizeof(soc_defragment_block_t) *
                 (ecmp_group_tbl_ptr->idx_max + 1), "defragment block array");
        }
    } else if (ecmp_level == 2) {
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
                block_array = sal_alloc(sizeof(soc_defragment_block_t) *
                        (ecmp_group_tbl_ptr->idx_maxused -
                         ecmp_group_tbl_ptr->split_idx + 1),
                        "defragment block array");
            } else
#endif
            {
                block_array = sal_alloc(sizeof(soc_defragment_block_t) *
                     (ecmp_group_tbl_ptr->split_maxused + 1),
                     "defragment block array");
            }
        } else
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (hecmp_enable) {
            block_array = sal_alloc(sizeof(soc_defragment_block_t) *
                                    (ecmp_group_tbl_ptr->idx_maxused + 1),
                                    "defragment block array");
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            /* Allocate block array of size equal to number of used entries at level 2 */
            block_array = sal_alloc(sizeof(soc_defragment_block_t) *
                 (ecmp_group_tbl_ptr->idx_maxused -
                  ecmp_group_tbl_ptr->idx_min + 1),
                 "defragment block array");
        }
    } else {
        block_array = sal_alloc(sizeof(soc_defragment_block_t) *
             (ecmp_group_tbl_ptr->idx_maxused + 1), "defragment block array");
    }

    /* Get the ECMP table base_ptr and max group size of all the active
     * ECMP groups
     */
    block_count = 0;
    if (SOC_IS_TRIUMPH3(unit) || BCM_XGS3_L3_MAX_ECMP_MODE(unit)) {
        ecmp_group_idx_increment = _BCM_SINGLE_WIDE;
    } else {
        ecmp_group_idx_increment = _BCM_DOUBLE_WIDE;
    }

    if (soc_feature(unit, soc_feature_l3_ecmp_1k_groups)) {
        uint32 ing_config_2;
        uint8 ecmp_hash_16bits = 1;
        if (SOC_REG_IS_VALID(unit, ING_CONFIG_2r)) {
            rv = READ_ING_CONFIG_2r(unit, &ing_config_2);
            if (SOC_FAILURE(rv)) {
                sal_free(block_array);
                return rv;
            }
            ecmp_hash_16bits =
                soc_reg_field_get(unit, ING_CONFIG_2r, ing_config_2,
                                  ECMP_HASH_16BITSf);
        }

        if (ecmp_hash_16bits && 
            soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTRf)) {
            base_ptr_f = BASE_PTRf;
        } else {
            base_ptr_f = BASE_PTR_0f;
        }
    } else if (soc_mem_field_valid(unit, L3_ECMP_COUNTm, BASE_PTR_0f)) {
        base_ptr_f = BASE_PTR_0f;
    } else {
        base_ptr_f = BASE_PTRf;
    }
    
    if (ecmp_level == 2) {
        min_idx = ecmp_group_tbl_ptr->idx_min;
        max_idx = ecmp_group_tbl_ptr->idx_maxused;
        free_base_ptr = min_idx;
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
            max_idx = ecmp_group_tbl_ptr->split_maxused;
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
                min_idx = ecmp_group_tbl_ptr->split_idx;
                max_idx = ecmp_group_tbl_ptr->idx_maxused;
            }
#endif
        } else 
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (hecmp_enable) {
            min_idx = soc_mem_index_min(unit, L3_ECMP_COUNTm);
            max_idx = ecmp_group_tbl_ptr->idx_maxused;
            ecmp_min_idx = soc_mem_index_count(unit, L3_ECMPm)/2;
            ecmp_max_idx = soc_mem_index_count(unit, L3_ECMPm);
            free_base_ptr = ecmp_min_idx;
        } else 
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            free_base_ptr = soc_mem_index_count(unit, L3_ECMPm)/2;
        }
        ecmp_tbl_size = soc_mem_index_count(unit, L3_ECMPm)/2;
    } else if (ecmp_level == 1) {
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
            min_idx = ecmp_group_tbl_ptr->split_idx;
            max_idx = ecmp_group_tbl_ptr->idx_maxused;
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                    BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
                min_idx = ecmp_group_tbl_ptr->idx_min;
                max_idx = ecmp_group_tbl_ptr->split_maxused;
            }
#endif
            ecmp_tbl_size = soc_mem_index_count(unit, L3_ECMPm);
            if (soc_feature(unit, soc_feature_td3_style_riot)) {
                ecmp_tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp);
                free_base_ptr = ecmp_tbl_ptr->split_idx;
            } else {
                free_base_ptr = soc_mem_index_count(unit, L3_ECMPm)/2;
            }
            vp_lag_base_idx = min_idx;
        } else 
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (hecmp_enable) {
            min_idx = ecmp_group_tbl_ptr->idx_min;
            max_idx = ecmp_group_tbl_ptr->idx_max;
            ecmp_min_idx = 0;
            ecmp_max_idx = soc_mem_index_max(unit, L3_ECMPm)/2;
            free_base_ptr = ecmp_min_idx;
            ecmp_tbl_size = soc_mem_index_count(unit, L3_ECMPm)/2;
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            min_idx = ecmp_group_tbl_ptr->idx_min;
            /*
             * Using table max index since we do not keep track of highest
             * index used at this level 1
             */
            max_idx = ecmp_group_tbl_ptr->idx_max;
            free_base_ptr = 0;
            ecmp_tbl_size = soc_mem_index_count(unit, L3_ECMPm)/2;
        }
    } else {
        min_idx = ecmp_group_tbl_ptr->idx_min;
        max_idx = ecmp_group_tbl_ptr->idx_maxused;
        ecmp_tbl_size = soc_mem_index_count(unit, L3_ECMPm);
        free_base_ptr = 0;
    }

    for (i = min_idx; i <= max_idx; i += ecmp_group_idx_increment) {

#ifdef BCM_TRIDENT2_SUPPORT
        /* Skip vp_lag checks at level 2 as vp lag entries are only at level 1 */
        if (soc_feature(unit, soc_feature_vp_lag) && (ecmp_level != 2)) {
            int is_vp_lag, is_used, has_member;
            rv = bcm_td2_vp_lag_status_get(unit, i - vp_lag_base_idx,
                    &is_vp_lag, &is_used, &has_member);
            if (BCM_FAILURE(rv)) {
                sal_free(block_array);
                return rv;
            }
            if (is_vp_lag) {
                /* Skip VP LAG entries */
                continue;
            }
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        if (!BCM_XGS3_L3_ENT_REF_CNT(ecmp_group_tbl_ptr, i)) {
            continue;
        }

        rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, i, &ecmp_group_entry);
        if (SOC_FAILURE(rv)) {
            sal_free(block_array);
            return rv;
        }
        base_ptr =
            soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry, base_ptr_f);
        rv = bcm_xgs3_l3_egress_ecmp_max_paths_get(unit,
                i + BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit), NULL, &max_paths);
        if (BCM_FAILURE(rv)) {
            sal_free(block_array);
            return rv;
        }

#if defined(BCM_TOMAHAWK_SUPPORT)
        /* In HECMP mode, there are 3 kinds of ECMP_GROUP, and they can be
         * coexisted, so we need to handle mixed ecmp group case;
         *   ecmp_group_flags:
         *       0       : ecmp_index  [0  ~ 16K-1]  nh-hop
         *       OVERLAY : ecmp_index  [0  ~ 8K-1]   ecmp_grp_ptr, Level 1
         *       UNDERLAY: ecmp_index  [8K ~ 16K-1]  nh-hop
         *   ecmp_index range between [8K ~ 16K-1] can't be ecmp_grp_ptr.
         *
         * So for a given ecmp_index in range [8K ~ 16K-1], the ecmp can
         * be either level 1 group or level 2 group.
 */
        if (hecmp_enable) {
            /* Adjust free_base_ptr when group member across the boundaries */
            if (ecmp_min_idx > 0 &&
                base_ptr < ecmp_min_idx &&
                (base_ptr + max_paths) > ecmp_min_idx) {
                free_base_ptr = (base_ptr + max_paths);
                continue;
            }

            /* Ignore out of range groups */
            if (base_ptr < ecmp_min_idx || base_ptr > ecmp_max_idx) {
                continue;
            }

            if ((base_ptr + max_paths) > ecmp_max_idx) {
                continue;
            }
        }
#endif

        block_array[block_count].base_ptr = base_ptr;
        block_array[block_count].size = max_paths;
        block_array[block_count].group = i;
        block_count++;
    }

#ifdef BCM_TRIDENT2_SUPPORT
    /* In Trident2, the VP LAG and ECMP features share the same group and
     * member tables. Defragmentation of the member table also requires
     * the group base_ptr and group size of all the active VP LAGs.
     */
    if (soc_feature(unit, soc_feature_vp_lag)) {
        int vp_id_min = -1, vp_id_max = -1;
        int min_vp_lag_id, max_vp_lag_id;
        int is_vp_lag, is_used, has_member;

        rv = _bcm_esw_trunk_chip_info_vp_resource_get(unit, &vp_id_min,
                                                      &vp_id_max, NULL);
        if (BCM_FAILURE(rv)) {
            sal_free(block_array);
            return rv;
        }
        if (vp_id_min != -1 && vp_id_max != -1) {
            min_vp_lag_id = 0;
            max_vp_lag_id = vp_id_max - vp_id_min;
            for (i = min_vp_lag_id; i <= max_vp_lag_id; i++) {
                rv = bcm_td2_vp_lag_status_get(unit, i, &is_vp_lag, &is_used,
                        &has_member);
                if (BCM_FAILURE(rv)) {
                    sal_free(block_array);
                    return rv;
                }
                if (!is_vp_lag) {
                    sal_free(block_array);
                    return BCM_E_INTERNAL;
                }
                if (!is_used) {
                    continue;
                }
                if (!has_member) {
                    continue;
                }

                rv = READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, i + vp_lag_base_idx,
                        &ecmp_group_entry);
                if (SOC_FAILURE(rv)) {
                    sal_free(block_array);
                    return rv;
                }
                block_array[block_count].base_ptr =
                    soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry,
                            BASE_PTRf);
                block_array[block_count].size = 1 +
                    soc_L3_ECMP_COUNTm_field32_get(unit, &ecmp_group_entry,
                            COUNTf);
                block_array[block_count].group = i + vp_lag_base_idx;
                block_count++;
            }
        }
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    /* Get the defragmentation buffer base_ptr and size */
    reserved_block.base_ptr =
        _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->base_ptr;
    reserved_block.size =
        _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->size;
    reserved_block.group = -1;

    /* Set functions to copy and clear ECMP member entries */
    member_op.member_copy = _bcm_tr2_l3_ecmp_member_copy;
    member_op.member_clear = _bcm_tr2_l3_ecmp_member_clear;

    /* Set function to update ECMP group's base_ptr */
    group_op.group_base_ptr_update = _bcm_tr2_l3_ecmp_group_base_ptr_update;

    /* Call defragmentation routine */
    rv = soc_defragment(unit, block_count, block_array, &reserved_block,
            ecmp_tbl_size, &member_op, &group_op, free_base_ptr);
    if (SOC_FAILURE(rv)) {
        sal_free(block_array);
        return rv;
    }

    sal_free(block_array);
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment
 * Purpose:
 *      Defragment L3 ECMP table.
 * Parameters:
 *      unit - (IN)  SOC unit number.
 * Returns:
 *      BCM_E_XXX.
 */
int
bcm_tr2_l3_ecmp_defragment(int unit)
{
    int rv;

    L3_LOCK(unit);

#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
    if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
        rv = bcm_tr2_l3_ecmp_defragment_no_lock(unit, 2);

        if (BCM_FAILURE(rv)){
            L3_UNLOCK(unit);
            return rv;
        }
        rv = bcm_tr2_l3_ecmp_defragment_no_lock(unit, 1);
        if (BCM_FAILURE(rv)){
            L3_UNLOCK(unit);
            return rv;
        }

    } else
#endif
    {
        rv = bcm_tr2_l3_ecmp_defragment_no_lock(unit, 0);
    }

    L3_UNLOCK(unit);

    return rv;
}

#ifdef BCM_WARM_BOOT_SUPPORT

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_buffer_wb_alloc_size_get
 * Purpose:
 *      Get level 2 warm boot scache size for ECMP defragmentation
 *      buffer parameters.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      size - (OUT) Allocation size.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_l3_ecmp_defragment_buffer_wb_alloc_size_get(int unit, int *size) 
{
    *size = 0;

    /* Allocate for buffer base_ptr */
    *size += sizeof(int);

    /* Allocate for buffer size */
    *size += sizeof(int);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_buffer_sync
 * Purpose:
 *      Store ECMP defragmentation buffer parameters into scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_l3_ecmp_defragment_buffer_sync(int unit, uint8 **scache_ptr, int offset)
{
    int value;
    SOC_SCACHE_DUMP_DECL;
    /* Store buffer base_ptr */
    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                          *scache_ptr - offset);

    value = _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->base_ptr;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_defragment_buffer_base_ptr");
    SOC_SCACHE_DUMP_DATA_HEAD((*scache_ptr), sizeof(int));
    SOC_SCACHE_DUMP_DATA_Vd((*scache_ptr), int);
    SOC_SCACHE_DUMP_DATA_TAIL();
    SOC_SCACHE_DUMP_DATA_END();
    (*scache_ptr) += sizeof(int);

    /* Store buffer base_ptr */
    value = _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->size;
    sal_memcpy((*scache_ptr), &value, sizeof(int));
    SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_defragment_buffer_size");
    SOC_SCACHE_DUMP_DATA_HEAD((*scache_ptr), sizeof(int));
    SOC_SCACHE_DUMP_DATA_Vd((*scache_ptr), int);
    SOC_SCACHE_DUMP_DATA_TAIL();
    SOC_SCACHE_DUMP_DATA_END();
    (*scache_ptr) += sizeof(int);

    SOC_SCACHE_DUMP_STOP();

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tr2_l3_ecmp_defragment_buffer_recover
 * Purpose:
 *      Recover ECMP defragment buffer parameters from scache.
 * Parameters:
 *      unit - (IN) SOC unit number.
 *      scache_ptr - (IN/OUT) Scache pointer
 *      offset - (IN) Scache offset
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr2_l3_ecmp_defragment_buffer_recover(int unit, uint8 **scache_ptr,
                                          int offset)
{
    int value;
    SOC_SCACHE_DUMP_DECL;

    SOC_SCACHE_DUMP_START(unit, bcm_module_name(unit, BCM_MODULE_L3),
                          *scache_ptr - offset);

    /* Recover defragmentation buffer base_ptr */
    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_defragment_buffer_base_ptr");
    SOC_SCACHE_DUMP_DATA_HEAD((*scache_ptr), sizeof(int));
    SOC_SCACHE_DUMP_DATA_Vd((*scache_ptr), int);
    SOC_SCACHE_DUMP_DATA_TAIL();
    SOC_SCACHE_DUMP_DATA_END();
    _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->base_ptr = value;
    (*scache_ptr) += sizeof(int);

    /* Recover defragmentation buffer size */
    sal_memcpy(&value, (*scache_ptr), sizeof(int));
    SOC_SCACHE_DUMP_DATA_BEGIN("l3_ecmp_defragment_buffer_size");
    SOC_SCACHE_DUMP_DATA_HEAD((*scache_ptr), sizeof(int));
    SOC_SCACHE_DUMP_DATA_Vd((*scache_ptr), int);
    SOC_SCACHE_DUMP_DATA_TAIL();
    SOC_SCACHE_DUMP_DATA_END();
    _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->size = value;
    (*scache_ptr) += sizeof(int);

    SOC_SCACHE_DUMP_STOP();

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     bcm_tr2_l3_ecmp_defragment_buffer_sw_dump
 * Purpose:
 *     Displays ECMP defragmentation buffer information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcm_tr2_l3_ecmp_defragment_buffer_sw_dump(int unit)
{
    LOG_CLI((BSL_META_U(unit,
                        "  ECMP Defragment Buffer: base_ptr = %d, size = %d\n"),
             _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->base_ptr,
             _bcm_tr2_l3_ecmp_defragment_buffer_info[unit]->size));
    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#else /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
typedef int bcm_esw_triumph2_l3_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

